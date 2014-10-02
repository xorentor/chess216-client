#include "common.h"
#include "client.h"
#include "controller.h"
#include "game.h"
#include "gfx.h"
#include "sha256.h"

GFX::GFX()
{
	// init pointers
	screen = NULL;
	fieldSelected = false;
	generatedBoard = 0;
	generateReversed = 0;
	pieces = NULL;
}

GFX::~GFX()
{
    int i;

	for( i = 0; i < 12; i++ ) {
		if( pieces[ i ] != NULL ) {
            free( pieces[ i ] );
        }
	}

    if( pieces != NULL ) {
        free( pieces );
    }
}

bool GFX::Init()
{
	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 ) {
	    return false;   
    }

    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    if( screen == NULL ) {
	    return false;
    }

    SDL_WM_SetCaption( "Chess216", NULL );

	// load pieces
	LoadImage( "gfx/pieces.png" );

	MapPieces();		

    return true;
}

void GFX::LoadImage( const char *filename )
{
    SDL_Surface *loadedImage = NULL;

    SDL_Surface *optimizedImage = NULL;

    loadedImage = IMG_Load( filename );
	
    if( loadedImage != NULL ) {
       	optimizedImage = SDL_DisplayFormat( loadedImage );

       	SDL_FreeSurface( loadedImage );

       	if( optimizedImage != NULL )
       	{
	            	SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 234, 10, 134 ) );
       	}
	}
	
	//SDL_SetAlpha( optimizedImage, SDL_SRCALPHA, 150 );

	PushImage( optimizedImage );
}

void GFX::SDL_MoveXY( SDL_Surface *srf, int top, int left )
{
	static SDL_SysWMinfo pInfo;

	#ifdef WIN32
		RECT r;
	#endif 
		SDL_VERSION(&pInfo.version);
		SDL_GetWMInfo(&pInfo);

	#ifdef WIN32
		GetWindowRect(pInfo.window, &r);
		SetWindowPos(pInfo.window, 0,left,top, 0, 0, SWP_NOSIZE);
		SDL_UpdateRect(srf,0,0,0,0);
	#else
		pInfo.info.x11.lock_func();
		XMoveWindow(pInfo.info.x11.display, pInfo.info.x11.wmwindow,top,left);
		pInfo.info.x11.unlock_func();
		SDL_UpdateRect(srf,0,0,0,0);
	#endif
}

void GFX::PushImage( SDL_Surface *image )
{
	Image_t *img;
	img = new Image_t;
	img->surface = image;
	img->inuse = true;
	images.push_back( img );
}

void GFX::ApplySurface( const int &x, const int &y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;

    if( SDL_BlitSurface( source, clip, destination, &offset ) != 0 ) {
		std::cout << "fail blit " << std::endl;
    }
}

bool GFX::Flip()
{
   	if( SDL_Flip( screen ) == -1 ) {
		return false;
    }

	return true;
}

void GFX::ClearScreen()
{
	SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
}

void GFX::CleanImages()
{
	for( std::vector<Image_t *>::iterator it = images.begin(); it != images.end(); it++ ) {
		if( !(*it)->inuse ) {
			SDL_FreeSurface( (*it)->surface );
			(*it)->surface = NULL;
			delete *it;
			it = images.erase( it );
		}
	}
}

void GFX::Run( Piece_t **pieces )
{
	ClearScreen();
	GenerateBoard( false );
	Update( pieces );
}

void GFX::Update( Piece_t **pieces )
{
	for( int i = 0; i < 32; i++ ) {
		// highlight square
		if( pieces[ i ]->isSelected ) {
			SDL_Rect fieldRect;
			fieldRect.x = BOARD_SQUARE_WH * pieces[ i ]->xpos;
			fieldRect.y = BOARD_SQUARE_WH * pieces[ i ]->ypos;
			fieldRect.w = BOARD_SQUARE_WH;
			fieldRect.h = BOARD_SQUARE_WH;

			SDL_FillRect( screen, &fieldRect, SDL_MapRGB( screen->format, 0, 128, 0 ) );
		}
		
		if( pieces[ i ]->inPlay ) {
			ApplySurface( pieces[ i ]->x, pieces[ i ]->y, images[0]->surface, screen, pieces[ i ]->skin );
        }
	}
}

bool GFX::GenerateBoard( const bool reverse )
{
	SDL_Rect board;

	for( int i = 0; i < 8; i++ ) {
		for( int j = 0; j < 8; j++ ) {
			board.x = BOARD_SQUARE_WH * j;
			board.y = BOARD_SQUARE_WH * i;
			board.w = BOARD_SQUARE_WH;
			board.h = BOARD_SQUARE_WH;
	
			if( !reverse && !generateReversed )
			{
				if( j % 2 == 0 && i % 2 != 0 ) {
					SDL_FillRect( screen, &board, SDL_MapRGB( screen->format, DARK_COLOR_R, DARK_COLOR_G, DARK_COLOR_B ) );
			    }
				if( j % 2 != 0 && i % 2 == 0 ) {
					SDL_FillRect( screen, &board, SDL_MapRGB( screen->format, DARK_COLOR_R, DARK_COLOR_G, DARK_COLOR_B ) );
                }
				generateReversed = false;
			} else {
				if( j % 2 == 0 && i % 2 == 0 ) {
					SDL_FillRect( screen, &board, SDL_MapRGB( screen->format, DARK_COLOR_R, DARK_COLOR_G, DARK_COLOR_B ) );
			    }
				if( j % 2 != 0 && i % 2 != 0 ) {
					SDL_FillRect( screen, &board, SDL_MapRGB( screen->format, DARK_COLOR_R, DARK_COLOR_G, DARK_COLOR_B ) );
                }
				generateReversed = true;
			}
		}
	}

	return true;
}

bool GFX::SwapBoard( void )
{
	if( generateReversed ) {
		generateReversed = false;
	} else {
		generateReversed = true;
    }

	return( GenerateBoard( false ) );	
}

#define PD( a, b, c, d, p )\
    pieces[ p ]->x = a;\
    pieces[ p ]->y = b;\
    pieces[ p ]->w = c;\
    pieces[ p ]->h = d;
void GFX::MapPieces()
{
    int i;

	pieces = (SDL_Rect **) malloc( sizeof(void*) * 12 );
	for( i = 0; i < 12; i++ ) {
		pieces[ i ] = (SDL_Rect *)malloc( sizeof( SDL_Rect ) );
	}

	PD( 65, 66, 85, 91, BLACK_KING );
	PD( 65, 213, 85, 91, WHITE_KING );
	PD( 228, 68, 98, 89, BLACK_QUEEN );
	PD( 228, 212, 98, 89, WHITE_QUEEN );
	PD( 411, 67, 69, 90, BLACK_ROOK );
	PD( 411, 212, 69, 90, WHITE_ROOK );
	PD( 567, 67, 89, 100, BLACK_BISHOP );
	PD( 567, 212, 89, 100, WHITE_BISHOP );
	PD( 738, 67, 82, 100, BLACK_KNIGHT );
	PD( 738, 212, 82, 100, WHITE_KNIGHT );
	PD( 918, 67, 60, 100, BLACK_PAWN );
	PD( 918, 212, 60, 100, WHITE_PAWN );
}
