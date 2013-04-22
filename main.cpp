#include <iostream>
#include <vector>
#include <gtk/gtk.h>
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <SDL/SDL_image.h>

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "common.h"
#include "client.h"
#include "controller.h"
#include "game.h"
#include "gtk.h"
#include "gfx.h"

GFX gfx;
SDL_Event event;
SDL_Rect **pieces;
SDL_Surface *screen;
std::vector<Image_t *> *images;
Game game;
int quit = 0;
int frame = 0;
int startTicks = 0;
GTK gtk;

void sock_init( int *sd )
{
	int portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;

    	portno = 5777; 
    	*sd = socket(AF_INET, SOCK_STREAM, 0);
    	if( *sd < 0 ) 
        	printf("ERROR opening socket");
    	server = gethostbyname("localhost"); 
    	if (server == NULL) {
        	printf("ERROR, no such host\n");
        	exit(0);
    	}
    	bzero((char *) &serv_addr, sizeof(serv_addr));
    	serv_addr.sin_family = AF_INET;
    	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    	serv_addr.sin_port = htons(portno);
    	if( connect( *sd,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) < 0 ) 
       		printf("ERROR connecting");
}

int gtk_init( Controller *controller )
{
	gtk.Init( controller );
}

int sdl_init( Controller *controller )
{
	if( !gfx.Init() )
	{
		printf("fail");
		exit(0);
	}

	screen = gfx.GetScreen();
	gfx.SDL_MoveXY( screen, 0, 0 );

	pieces = gfx.GetPieces();	
	images = gfx.GetImages();

	game.Init( pieces, controller );
//	controller->SetGame( &game );

	return 0;
}

int sdl_loop( void *l )
{	
	gfx.Run( game.GetPieces() );
	/*			
		for( int i = 0; i < 12; i++ )
		{
	    		gfx.ApplySurface( 300, 300, (*images)[0]->surface, screen, pieces[ i ] );
		}
	*/	
       	while( SDL_PollEvent( &event ) )
       	{
		switch( event.type )
			{
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

		gfx.Flip();

		SDL_Delay( 1000 / 60 );
		frame++;
		if( ( SDL_GetTicks() - startTicks ) > 1000 )
		{
			startTicks = SDL_GetTicks();
			frame = 0;
		}

	return 1;
}

void *game_thread( void *controller ) {
	sdl_init( (Controller *)controller ); 
	gtk_init( (Controller *)controller );

   	gtk_idle_add( sdl_loop, NULL ); 
   	gtk_main();
}

void *listener_thread( void *controller )
{
	int buffLen;
	char readBuffer[ BUFFER_LEN ];
	while( ( (Controller *)controller )->Stop() != 1 ) {
                buffLen = read( *( ( (Controller *)controller )->GetDescriptor() ), readBuffer, BUFFER_LEN );
		printf( "read\n" );
		if( buffLen <= 0 ) {
			printf("stopping listener\n");
			break;
		}
			// TODO: this could be removed if we point this structure to readBuffer
        		PacketData_t pd;
		 	memcpy( &pd.command, &readBuffer, sizeof( pd.command ) );
                	pd.data = readBuffer + sizeof( pd.command );

			switch( (int )pd.command ) {
				case CMD_LOGIN:
					if( (int )( (ServerByte_t *)pd.data )->byte == CMD_LOGIN_PARAM_DETAILS_OK ) {
						( (Controller *)controller )->GTKSysMsg( CMD_LOGIN_PARAM_DETAILS_OK ); }

					else if( (int )( (ServerByte_t *)pd.data )->byte == CMD_LOGIN_PARAM_DETAILS_ERR ) {
						( (Controller *)controller )->GTKSysMsg( CMD_LOGIN_PARAM_DETAILS_ERR ); }
					break;
				case CMD_GAME_CREATE:

					if( (int )( (ServerTwoBytes_t *)pd.data )->byte0 == CMD_GAME_CREATE_PARAM_OK ) {
						char gamename[ 0x20 ];
						sprintf( gamename, "Game %d\n", (int )( (ServerTwoBytes_t *)pd.data )->byte1 ); 
						( (Controller *)controller )->GTKAppendGameListItem( gamename, &( (ServerTwoBytes_t *)pd.data )->byte1 );
						( (Controller *)controller )->GTKSysMsg( CMD_GAME_CREATE_PARAM_OK ); 
					} 
					break;
				case CMD_GAME_JOIN:
					if( (int )( (ServerTwoBytes_t *)pd.data )->byte0 == CMD_GAME_JOIN_PARAM_OK ) {
						char gamename[ 0x20 ];
						sprintf( gamename, "Game %d\n", (int )( (ServerTwoBytes_t *)pd.data )->byte1 );
						( (Controller *)controller )->GTKSetGamename( gamename, &( (ServerTwoBytes_t *)pd.data )->byte1 );
						( (Controller *)controller )->GTKSetButtonSitActive();
					} 
					break;
				case CMD_GAME_SIT:
					if( (int )( (GameSitServerData_t *)pd.data )->slot == COLOR_WHITE ) {
						printf("white sit ok\n");
						( (Controller *)controller )->GTKSetPlayer1( ( (GameSitServerData_t *)pd.data )->username );
					}
					else if( (int )( (GameSitServerData_t *)pd.data )->slot == COLOR_BLACK ) {
						( (Controller *)controller )->GTKSetPlayer2( ( (GameSitServerData_t *)pd.data )->username );
					}

					if( (int )( (GameSitServerData_t *)pd.data )->gameBegin == CMD_GAME_BEGIN_PARAM_OK ) {
						( (Controller *)controller )->GTKSysMsg( CMD_GAME_BEGIN_PARAM_OK ); 
					}
					break;
				case CMD_GAME_MOVEPIECE:
					printf("color to move: %d\n", (int )( (GamePieceMoveSrv_t *)pd.data )->next == COLOR_WHITE );
					if( (int )( (GamePieceMoveSrv_t *)pd.data )->next == COLOR_WHITE )
						( (Controller *)controller )->GTKSysMsg( CMD_GAME_PARAM_NEXTWHITE ); 
					else
						( (Controller *)controller )->GTKSysMsg( CMD_GAME_PARAM_NEXTBLACK ); 
					if( (int )( (GamePieceMoveSrv_t *)pd.data )->checkMate )
						( (Controller *)controller )->GTKSysMsg( CMD_GAME_PARAM_CHECKMATE ); 
					game.FinalMovePiece( (int )( (GamePieceMoveSrv_t *)pd.data )->pieceId , (int )( (GamePieceMoveSrv_t *)pd.data )->xdest, (int )( (GamePieceMoveSrv_t *)pd.data )->ydest );
					break;

			}
			printf("received %d\n", pd.command);
	}
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
