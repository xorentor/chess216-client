#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "common.h"
#include "client.h"
#include "controller.h"
#include "game.h"
#include "sha256.h"

Game::Game() 
{
}

Game::~Game()
{
}

const bool Game::Init(  SDL_Rect **skins, Controller *controller ) 
{
    this->controller = controller;
	this->skins = skins;

	// 32 pointers to pieces
	listPieces = (Piece_t **) malloc( sizeof(int) * 32 );
	int i = 0;
	// white Pawns
	for( ; i < 8; i++ )
	{
		AddPiece( listPieces, i, i, 6, WHITE_PAWN, COLOR_WHITE );
	}

	// black Pawns
	for(  ; i < 16; i++ )
	{
		AddPiece( listPieces, i, (i-8), 1, BLACK_PAWN, COLOR_BLACK );
	}

	// rooks
	AddPiece( listPieces, i++, 7, 7, WHITE_ROOK, COLOR_WHITE );
	AddPiece( listPieces, i++, 0, 7, WHITE_ROOK, COLOR_WHITE );
	AddPiece( listPieces, i++, 7, 0, BLACK_ROOK, COLOR_BLACK );
	AddPiece( listPieces, i++, 0, 0, BLACK_ROOK, COLOR_BLACK );
	
	// knights
	AddPiece( listPieces, i++, 1, 7, WHITE_KNIGHT, COLOR_WHITE );
	AddPiece( listPieces, i++, 6, 7, WHITE_KNIGHT, COLOR_WHITE );
	AddPiece( listPieces, i++, 1, 0, BLACK_KNIGHT, COLOR_BLACK );
	AddPiece( listPieces, i++, 6, 0, BLACK_KNIGHT, COLOR_BLACK );

	// bishops
	AddPiece( listPieces, i++, 2, 7, WHITE_BISHOP, COLOR_WHITE );
	AddPiece( listPieces, i++, 5, 7, WHITE_BISHOP, COLOR_WHITE );
	AddPiece( listPieces, i++, 2, 0, BLACK_BISHOP, COLOR_BLACK );
	AddPiece( listPieces, i++, 5, 0, BLACK_BISHOP, COLOR_BLACK );

	// queens
	AddPiece( listPieces, i++, 3, 7, WHITE_QUEEN, COLOR_WHITE );
	AddPiece( listPieces, i++, 3, 0, BLACK_QUEEN, COLOR_BLACK );

	// kings
	AddPiece( listPieces, i++, 4, 7, WHITE_KING, COLOR_WHITE );
	AddPiece( listPieces, i++, 4, 0, BLACK_KING, COLOR_BLACK );

	return true;
}

void Game::SrvInitPieces( char *data )
{
	int index = 0;

	for( int i = 0; i < 32; i++ )
	{
		listPieces[ i ]->xpos = data[ index ];
		listPieces[ i ]->x = data[ index++ ] * BOARD_SQUARE_WH;
		listPieces[ i ]->ypos = data[ index ];
		listPieces[ i ]->y = data[ index++ ] * BOARD_SQUARE_WH;
		//listPieces[ i ]->ID = data[ index++ ];
		index++;
		listPieces[ i ]->skinID = data[ index++ ];
		listPieces[ i ]->inPlay = data[ index++ ] & 0x1 ? true : false;
	}
}

bool Game::EnPassant( Piece_t *piece, const int &xdest, const int &ydest )
{
	if( piece->color == COLOR_BLACK ) {
		if( lastMove.skinID == WHITE_PAWN && lastMove.srcY == 6 && lastMove.destY == 4 && piece->skinID == BLACK_PAWN && ( piece->xpos == lastMove.destX + 1 || piece->xpos == lastMove.destX - 1 ) && piece->ypos == lastMove.destY && xdest == lastMove.destX && ydest == ( lastMove.destY + 1 ) ) 
			return true;
	} else {
		if( lastMove.skinID == BLACK_PAWN && lastMove.srcY == 1 && lastMove.destY == 3 && piece->skinID == WHITE_PAWN && ( piece->xpos == lastMove.destX + 1 || piece->xpos == lastMove.destX - 1 ) && piece->ypos == lastMove.destY && xdest == lastMove.destX && ydest == ( lastMove.destY - 1 ) ) 
			return true;
	}

	return false;
}

void Game::FinalMovePiece( const int &p, const int &xdest, const int &ydest )
{
	Piece_t *piece = NULL;

	for( int i = 0; i < 32; i++ ) {
		if( !listPieces[ i ]->inPlay )
			continue;

		if( listPieces[ i ]->ID == p ) {
			piece = listPieces[ i ];
		}			
	}
	
	if( piece == NULL )
		return;

	if( EnPassant( piece, xdest, ydest ) ) {
		for( int i = 0; i < 32; i++ ) {
			if( !listPieces[ i ]->inPlay )
				continue;

			if( listPieces[ i ]->xpos == xdest && listPieces[ i ]->ypos == piece->ypos ) {
				listPieces[ i ]->inPlay = false;
			}
		}		
	}

	lastMove.skinID = piece->skinID;
	lastMove.srcX = piece->xpos;
	lastMove.srcY = piece->ypos;
	lastMove.destX = xdest;
	lastMove.destY = ydest;

	// castle king's side
	if( xdest == ( piece->xpos + 2 ) && ydest == piece->ypos && piece->isInitialState && ( piece->skinID == BLACK_KING || piece->skinID == WHITE_KING ) ) {
		if( piece->skinID == WHITE_KING ) {
			listPieces[ 16 ]->xpos = piece->xpos + 1;
			listPieces[ 16 ]->x = BOARD_SQUARE_WH * ( xdest - 1 );
			listPieces[ 16 ]->isInitialState = false;
		}

		if( piece->skinID == BLACK_KING ) {
			listPieces[ 18 ]->xpos = piece->xpos + 1;
			listPieces[ 18 ]->x = BOARD_SQUARE_WH * ( xdest - 1 );
			listPieces[ 18 ]->isInitialState = false;
		}

		piece->xpos = xdest;	
		piece->x = BOARD_SQUARE_WH * xdest;
		return;
	}

	// cast queen's side
	if( xdest == ( piece->xpos - 2 ) && ydest == piece->ypos && piece->isInitialState && ( piece->skinID == BLACK_KING || piece->skinID == WHITE_KING ) ) {
		if( piece->skinID == WHITE_KING ) {
			listPieces[ 17 ]->xpos = piece->xpos - 1;
			listPieces[ 17 ]->x = BOARD_SQUARE_WH * ( xdest + 1 );
			listPieces[ 17 ]->isInitialState = false;
		}

		if( piece->skinID == BLACK_KING ) {
			listPieces[ 19 ]->xpos = piece->xpos - 1;
			listPieces[ 19 ]->x = BOARD_SQUARE_WH * ( xdest + 1 );
			listPieces[ 19 ]->isInitialState = false;
		}

		piece->xpos = xdest;	
		piece->x = BOARD_SQUARE_WH * xdest;
		piece->isInitialState = false;
		return;
	}

	// turn pawn into queen
	if( ( piece->skinID == BLACK_PAWN && piece->ypos == 6 && ydest == 7 ) || ( piece->skinID == WHITE_PAWN  && piece->ypos == 1 && ydest == 0 ) ) {
		printf("turn pawn\n");
		if( piece->skinID == WHITE_PAWN ) {
			piece->skinID = WHITE_QUEEN;
			piece->skin = skins[ WHITE_QUEEN ];
		} else if ( piece->skinID == BLACK_PAWN ) {
			piece->skinID = BLACK_QUEEN;
			piece->skin = skins[ BLACK_QUEEN ];
		}
	}

	for( int i = 0; i < 32; i++ ) {
		if( !listPieces[ i ]->inPlay )
			continue;

		if( listPieces[ i ]->xpos == xdest && listPieces[ i ]->ypos == ydest && listPieces[ i ]->color != piece->color ) {
			
			printf( "capture state piece: %d \n", listPieces[ i ]->ID );	
			listPieces[ i ]->inPlay = false;
			piece->xpos = xdest;
			piece->ypos = ydest;
			piece->x = BOARD_SQUARE_WH * xdest;
			piece->y = BOARD_SQUARE_WH * ydest;
			piece->isInitialState = false;
		
			simulatePiece = listPieces[ i ];
			return;
		}
	}	

	printf( "empty square\n" );
	piece->xpos = xdest;
	piece->ypos = ydest;
	piece->x = BOARD_SQUARE_WH * xdest;
	piece->y = BOARD_SQUARE_WH * ydest;
	piece->isInitialState = false;
}	

void Game::MouseInput( const int &xm, const int &ym )
{
	int x, y, i;	

	x = (int)(xm / BOARD_SQUARE_WH); 
	y = (int)(ym / BOARD_SQUARE_WH);


	for( int i = 0; i < 32; i++ )
	{
		if( listPieces[ i ]->isSelected && listPieces[ i ]->inPlay  )
		{
			controller->MovePiece( listPieces[ i ]->xpos, listPieces[ i ]->ypos, x, y );
			/*
			if( CheckMove( listPieces[ i ], x, y ) ) {
				if( !KingCheckSimulate( listPieces[ i ], x, y ) ) {
					FinalMovePiece( listPieces[ i ], x, y );
					if( listPieces[ i ]->color == COLOR_WHITE ) {
						if( KingCheckMate( COLOR_BLACK ) )
							printf("check mate\n");
					} else {
						if( KingCheckMate( COLOR_WHITE ) )
							printf("check mate\n");
					}
				}
			}

			listPieces[ i ]->isSelected = false;
			*/

			listPieces[ i ]->isSelected = false;
			goto done;
		}	
	}

	for( int i = 0; i < 32; i++ )
	{
		//if( !listPieces[ i ]->inPlay )
		//	break;

		if( x == listPieces[ i ]->xpos && y == listPieces[ i ]->ypos && listPieces[ i ]->inPlay )
		{
			listPieces[ i ]->isSelected = true;
			break;
		}
	}

	done:
	return;
}

void Game::AddPiece( Piece_t **listPieces, const int &i, const int &x, const int &y, const int &skin, const int &color ) 
{
	listPieces[ i ] = (Piece_t *) malloc( sizeof(Piece_t) );
	listPieces[ i ]->skin = skins[ skin ];
	listPieces[ i ]->x = BOARD_SQUARE_WH * x;
	listPieces[ i ]->y = BOARD_SQUARE_WH * y;
	listPieces[ i ]->xpos = x;
	listPieces[ i ]->ypos = y;
	listPieces[ i ]->inPlay = true;
	listPieces[ i ]->isSelected = false;
	listPieces[ i ]->ID = i;
	listPieces[ i ]->skinID = skin;
	listPieces[ i ]->color = color;
	listPieces[ i ]->isInitialState = true;
}

