#include "common.h"
#include "client.h"
#include "controller.h"
#include "game.h"
#include "gtk.h"
#include "gfx.h"
#include "sha256.h"

static GTK gtk;
static Game game;
static int frame = 0;
static int startTicks = 0;
int quit = 0;

void dprint( const char * format, ...) {
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr,format,ap);
	fflush(stderr);
	va_end(ap);
}

static void sock_init( int *sd ) {
	struct sockaddr_in serv_addr;
	struct hostent *server;

    *sd = socket( AF_INET, SOCK_STREAM, 0 );
    if( *sd < 0 ) { 
       	LM_ERR( "opening socket" );
    }
    server = gethostbyname( "localhost" ); 
    if( server == NULL ) {
        LM_ERR( "no such host" );
    }
    bzero( (char *) &serv_addr, sizeof(serv_addr) );
    serv_addr.sin_family = AF_INET;
    bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length );
    serv_addr.sin_port = htons(5777);
    if( connect( *sd,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) < 0 ) {
        LM_ERR("connect()\n");
    }
}

static int sdl_init( Controller *controller, GFX *gfx ) {
    SDL_Rect **pieces;
    SDL_Surface *screen;
    std::vector<Image_t *> *images;

	if( !gfx->Init() ) {
		LM_ERR("GFX init failed");
		exit(0);
	}

	screen = gfx->GetScreen();
	gfx->SDL_MoveXY( screen, 0, 0 );
	pieces = gfx->GetPieces();	
	images = gfx->GetImages();

	game.Init( pieces, controller );
//	controller->SetGame( &game );

	return 0;
}

static int sdl_loop( void *g ) {	
    SDL_Event event;
    GFX *gfx = (GFX *)g;

	gfx->Run( game.GetPieces() );
	/*			
		for( int i = 0; i < 12; i++ )
		{
	    		gfx.ApplySurface( 300, 300, (*images)[0]->surface, screen, pieces[ i ] );
		}
	*/	
    while( SDL_PollEvent( &event ) ) {
	    switch( event.type ) {
			case SDL_MOUSEBUTTONDOWN:
				int x, y;
				SDL_GetMouseState( &x, &y );
				game.MouseInput( x, y );
				//gfx.SwapBoard();
				break;

			case SDL_QUIT:
				break;
		}
    }

	gfx->Flip();

	SDL_Delay( 1000 / 60 );
	frame++;
	if( ( SDL_GetTicks() - startTicks ) > 1000 ) {
		startTicks = SDL_GetTicks();
		frame = 0;
	}

	return 1;
}

static void *game_thread( void *controller ) {
    GFX *gfx;

    gfx = (GFX *)malloc( sizeof( GFX ) );

	sdl_init( (Controller *)controller, gfx ); 
	gtk.Init( (Controller *)controller );

   	gtk_idle_add( sdl_loop, (void *)gfx ); 
   	gtk_main();

    return NULL;
}

static void *listener_thread( void *controller ) {
	int buffLen;
	char readBuffer[ BUFFER_LEN ];
    Controller *ctl = (Controller *)controller;
	char gamename[ 64 ];
    PacketData_t pd;

	while( ctl->Stop() != 1 ) {
        buffLen = read( *( ctl->GetDescriptor() ), readBuffer, BUFFER_LEN );
		if( buffLen <= 0 ) {
		    ctl->SetStop();
            break;
		}
			// TODO: this could be removed if we point this structure to readBuffer
            memset( &pd, 0, sizeof( pd ) );
		 	memcpy( &pd.command, &readBuffer, sizeof( pd.command ) );
           	pd.data = readBuffer + sizeof( pd.command );

			switch( (char )pd.command ) {
				case CMD_LOGIN:
					if( *(char *)pd.data == CMD_LOGIN_PARAM_DETAILS_OK ) {
						ctl->GTKSysMsg( CMD_LOGIN_PARAM_DETAILS_OK ); 
						ctl->GTKLoggedUser( ( (GameLoginSrv_t *)pd.data )->username );
				 		ctl->GTKSetElo( ( ( GameLoginSrv_t *)pd.data )->elorating );
						ctl->GTKHideLogin();
					} else if( *(char *)pd.data == CMD_LOGIN_PARAM_DETAILS_ERR ) {
						ctl->GTKSysMsg( CMD_LOGIN_PARAM_DETAILS_ERR ); 
                    }
					break;
				case CMD_GAME_CREATE:
					if( *(char *)(pd.data) == CMD_GAME_CREATE_PARAM_OK ) {
						memset( gamename, 0, sizeof( gamename ) );
						sprintf( gamename, "Game %d", *(char *)(pd.data+1) ); 
						ctl->GTKAppendGameListItem( gamename, (char *)(pd.data+1) );
						ctl->GTKSysMsg( CMD_GAME_CREATE_PARAM_OK ); 
					} else if( *(char *)(pd.data) == CMD_GAME_CREATE_PARAM_DELETE ) {
						LM_INFO("delete game %d\n", *(char *)(pd.data+1) );
						memset( gamename, 0, sizeof( gamename ) );
						sprintf( gamename, "Game %d", *(char *)(pd.data+1) ); 
						ctl->GTKRemoveGameListItem( gamename, (char *)(pd.data+1));
                    }	
					break;
				case CMD_GAME_JOIN:
					if( *(char *)pd.data == CMD_GAME_JOIN_PARAM_OK ) {
						memset( gamename, 0, sizeof( gamename ) );
						sprintf( gamename, "Game %d\n", *(char *)(pd.data+1) );
						ctl->GTKSetGamename( gamename, (char *)(pd.data+1), false );
						ctl->GTKSetButtonSitActive();
					} else { LM_INFO("Unable to join gameId: %d\n", *(char *)(pd.data+1) ); } 
					break;
				case CMD_GAME_SIT:
					if( (char )( (GameSitServerData_t *)pd.data )->slot == COLOR_WHITE ) {
						ctl->GTKSetPlayer1( ( (GameSitServerData_t *)pd.data )->username );
                        LM_INFO( "White sit OK" );
					} else if( (char )( (GameSitServerData_t *)pd.data )->slot == COLOR_BLACK ) {
						ctl->GTKSetPlayer2( ( (GameSitServerData_t *)pd.data )->username );
                        LM_INFO( "Black sit OK" );
					}

					if( (char )( (GameSitServerData_t *)pd.data )->gameBegin == CMD_GAME_BEGIN_PARAM_OK ) {
						ctl->GTKSysMsg( CMD_GAME_BEGIN_PARAM_OK ); 
					}
					break;
				case CMD_GAME_MOVEPIECE:
					LM_INFO("color to move: %d\n", (char )( (GamePieceMoveSrv_t *)pd.data )->next == COLOR_WHITE );
					if( (char )( (GamePieceMoveSrv_t *)pd.data )->next == COLOR_WHITE )
						ctl->GTKSysMsg( CMD_GAME_PARAM_NEXTWHITE ); 
					else
						ctl->GTKSysMsg( CMD_GAME_PARAM_NEXTBLACK ); 
					if( (char )( (GamePieceMoveSrv_t *)pd.data )->checkMate == CMD_GAME_PARAM_CHECKMATE_W )
						ctl->GTKSysMsg( CMD_GAME_PARAM_CHECKMATE_W ); 
					if( (char )( (GamePieceMoveSrv_t *)pd.data )->checkMate == CMD_GAME_PARAM_CHECKMATE_B )
						ctl->GTKSysMsg( CMD_GAME_PARAM_CHECKMATE_B ); 

					game.FinalMovePiece( (char )( (GamePieceMoveSrv_t *)pd.data )->pieceId , (char )( (GamePieceMoveSrv_t *)pd.data )->xdest, (char )( (GamePieceMoveSrv_t *)pd.data )->ydest );
					break;

				case CMD_GAME_INITIAL_PIECES:
					game.SrvInitPieces( (char *)pd.data );
					break;

				case CMD_GAME_STAND:
 					if( (char )( (GameStandServerData_t *)pd.data )->param == CMD_GAME_STAND_PARAM_OK ) {
    					if( (char )( (GameStandServerData_t *)pd.data )->slot == COLOR_WHITE ) {
	    					ctl->GTKSetPlayer1( "Sit" );
		    			}
				    	else if( (char )( (GameStandServerData_t *)pd.data )->slot == COLOR_BLACK ) {
			    			ctl->GTKSetPlayer2( "Sit" );
					    }
					}
					break;
				case CMD_GAME_TIMER:
					ctl->GTKSetTimer( ( (GameTimerSrv_t *)pd.data )->p1_min, ( (GameTimerSrv_t *)pd.data )->p1_sec, ( (GameTimerSrv_t *)pd.data )->p2_min, ( (GameTimerSrv_t *)pd.data )->p2_sec );
					break;
				case CMD_GAME_FINISHED:
					LM_INFO( "game finished" );
					switch( *(char *)pd.data ) {
						case CMD_GAME_FINISHED_DRAW:
							break;
						case COLOR_WHITE:
							break;
						case COLOR_BLACK:
							break;
					}

					ctl->GTKSetPlayer1( "Sit" );
					ctl->GTKSetPlayer2( "Sit" );
					ctl->GTKSetButtonSitInActive();
					ctl->GTKSetTimer( 10, 0, 10, 0 );
					ctl->GTKSetGamename( "No Name", NULL, true );
					break;
				case CMD_GAME_ELO:
			 		ctl->GTKSetElo( ( ( EloSrv_t *)pd.data )->elo_value );

					break;
			}
	}

    return NULL;
}

int main( int argc, char *argv[] ) {
	pthread_t tid[ 2 ];
	int sd;
	Controller controller;
	pthread_mutex_t mutex;

    pthread_mutex_init( &mutex, NULL );
	sock_init( &sd );

	controller.SetDescriptor( &sd );
	controller.SetMutex( &mutex );

    pthread_create( &(tid[ 0 ]), NULL, &game_thread, (void *)&controller );
    pthread_create( &(tid[ 1 ]), NULL, &listener_thread, (void *)&controller );

    pthread_join( tid[ 0 ], NULL );
    pthread_join( tid[ 1 ], NULL );

	return 0;
}
