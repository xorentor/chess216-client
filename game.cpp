#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "common.h"
#include "client.h"
#include "controller.h"
#include "game.h"

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

bool Game::CheckMove( Piece_t *piece, const int &xdest, const int &ydest )
{
	if( xdest < 0 || xdest > 7 || ydest < 0 || ydest > 7 )
		return false;

	if( piece->xpos == xdest && piece->ypos == ydest )
		return false;

	switch( piece->skinID )
	{
		case WHITE_KNIGHT:
		case BLACK_KNIGHT:
			return MoveKnight( piece, xdest, ydest );
			break;

		case WHITE_BISHOP:
		case BLACK_BISHOP:
			return MoveBishop( piece, xdest, ydest );	
			break;
		
		case WHITE_ROOK:
		case BLACK_ROOK:
			return MoveRook( piece, xdest, ydest );	
			break;

		case WHITE_QUEEN:
		case BLACK_QUEEN:
			return MoveQueen( piece, xdest, ydest );
			break;
		case WHITE_KING:
		case BLACK_KING:
			return MoveKing( piece, xdest, ydest );
			break;

		case WHITE_PAWN:
		case BLACK_PAWN:
			return MovePawn( piece, xdest, ydest );
			break;

		default:
			std::cout << "not selected" << std::endl;
			return false;
			break;
	}
}

bool Game::SquareFree( const int &x, const int &y )
{
	for( int i = 0; i < 32; i++ ) {
		if( !listPieces[ i ]->inPlay )
			continue;

		if( x == listPieces[ i ]->xpos && y == listPieces[ i ]->ypos )
			return false;
	}

	return true;
}

bool Game::SquareChecked( const int &x, const int &y, const int &color )
{
	for( int i = 0; i < 32; i++ ) {
		if( !listPieces[ i ]->inPlay )
			continue;

		// check against enemy color
		if( listPieces[ i ]->color == color )
			continue;

		if( CheckMove( listPieces[ i ], x, y ) )
			return true;
	}

	return false;
}

bool Game::MoveKing( Piece_t *piece, const int &xdest, const int &ydest )
{
	int *x, *y;
	x = &(piece->xpos);
	y = &(piece->ypos);

	if( xdest >= ( *x - 1 ) && xdest <= ( *x + 1 ) && ydest >= ( *y - 1 ) && ydest <= ( *y + 1 ) ) 
		return MovePiece( piece, xdest, ydest );

	// To castle:
	
	// king's side
	if( xdest == ( *x + 2 ) && ydest == *y && piece->isInitialState && ( ( piece->skinID == WHITE_KING && listPieces[ 16 ]->isInitialState ) || ( piece->skinID == BLACK_KING && listPieces[ 18 ]->isInitialState ) ) ) {
		if( SquareChecked( *x, *y, piece->color ) )
			return false;

		for( int i = 1; i <= 2; i++ ) {
			if( !SquareFree( *x + i, *y ) || SquareChecked( *x + i, *y, piece->color ) ) {
				printf( "position checked x %d y %d \n", *x + i, *y  );
				return false;
			}
		}
		printf( "castle king's side\n" );
		return true;
	}

	// queen's side
	if( xdest == ( *x - 2 ) && ydest == *y && piece->isInitialState && ( ( piece->skinID == WHITE_KING && listPieces[ 17 ]->isInitialState ) || ( piece->skinID == BLACK_KING && listPieces[ 19 ]->isInitialState ) ) ) {
		if( SquareChecked( *x, *y, piece->color ) )
			return false;

		for( int i = 1; i <= 3; i++ ) {
			if( !SquareFree( *x - i, *y ) || SquareChecked( *x - i, *y, piece->color ) ) {
				printf( "position checked x %d y %d \n", *x - i, *y  );
				return false;
			}
		}

		printf( "castle queen's side\n" );
		return true;
	}

	return false;
}

bool Game::KingCheck( const int &color )
{
	Piece_t *king;

	// get king of the same colour
	for( int i = 0; i < 32; i++ ) {
		if( !listPieces[ i ]->inPlay )
			continue;

		if( color == COLOR_WHITE && listPieces[ i ]->skinID == WHITE_KING ) {
			king = listPieces[ i ];
			break;
		}
		
		if( color == COLOR_BLACK && listPieces[ i ]->skinID == BLACK_KING ) {
			king = listPieces[ i ];
			break;
		}
	}

	// see if enemy pieces can reach king
	for( int i = 0; i < 32; i++ ) {
		if( !listPieces[ i ]->inPlay )
			continue;
		
		if( listPieces[ i ]->color == color )
			continue;

		if( CheckMove( listPieces[ i ], king->xpos, king->ypos ) )
			return true;	
	}
	
	return false;
}

// run post-move
bool Game::KingCheckMate( const int &color )
{
	Piece_t *king;

	if( color == COLOR_WHITE ) {
		king = listPieces[ 30 ];
		printf("white king  \n");
	}
	else {
		king = listPieces[ 31 ];
		printf("black king  \n");
	}

	if( !KingCheck( color ) ) {
		printf("king not checked color: %d \n", color );
		return false;
	}

	printf("king checked 3 \n");
	
	if( CheckMove( king, king->xpos - 1, king->ypos - 1 ) ) { 
		if( !KingCheckSimulate( king, king->xpos - 1, king->ypos - 1 ) ) {
			return false;
		}
	}

	printf("king checked 4 \n");

	if( CheckMove( king, king->xpos, king->ypos - 1 ) ) { 
		if( !KingCheckSimulate( king, king->xpos, king->ypos - 1 ) ) {
			return false;
		}
	}

	printf("king checked 5 \n");

	if( CheckMove( king, king->xpos + 1, king->ypos - 1 ) ) { 
		if( !KingCheckSimulate( king, king->xpos + 1, king->ypos - 1 ) ) {
			return false;
		}
	}

	printf("king checked 6 \n");

	if( CheckMove( king, king->xpos - 1, king->ypos ) ) { 
		if( !KingCheckSimulate( king, king->xpos - 1, king->ypos ) ) {
			return false;
		}
	}

	printf("king checked 7 \n");

	if( CheckMove( king, king->xpos + 1, king->ypos ) ) { 
		if( !KingCheckSimulate( king, king->xpos + 1, king->ypos ) ) {
			return false;
		}
	}

	printf("king checked 8 \n");

	if( CheckMove( king, king->xpos - 1, king->ypos + 1 ) ) { 
		if( !KingCheckSimulate( king, king->xpos - 1, king->ypos + 1 ) ) {
			return false;
		}
	}

	printf("king checked 9 \n");

	if( CheckMove( king, king->xpos, king->ypos + 1 ) ) { 
		if( !KingCheckSimulate( king, king->xpos, king->ypos + 1 ) ) {
			return false;
		}
	}

	printf("king checked 10 \n");

	if( CheckMove( king, king->xpos + 1, king->ypos + 1 ) ) { 
		if( !KingCheckSimulate( king, king->xpos + 1, king->ypos + 1 ) ) {
			return false;
		}
	}

	printf("king checked 11 \n");

	// if still checked now, we need to:
	// 1. capture the piece that checks AND do not get checked OR
	// 2. move to the square that will block checking AND do not get checked by other piece
	for( int i = 0; i < 32; i++ ) {
		if( !listPieces[ i ]->inPlay )
			continue;

		if( listPieces[ i ]->color != color )
			continue;

		for( int x = 0; x < 8; x++ ) {
			for( int y = 0; y < 8; y++ ) {
				if( CheckMove( listPieces[ i ], x, y ) ) {
					if( !KingCheckSimulate( listPieces[ i ], x, y ) ) {
						printf( "piece %d can prevent check \n", i );
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool Game::KingCheckSimulate( Piece_t *piece, const int &xdest, const int &ydest )
{
	int tx, ty;
	Piece_t *tPiece = NULL;
	bool checkedSquare = false;

	tx = piece->xpos;
	ty = piece->ypos;

	// capture piece temporarily
	for( int i = 0; i < 32; i++ ) {
		if( !listPieces[ i ]->inPlay )
			continue;

		if( listPieces[ i ]->xpos == xdest && listPieces[ i ]->ypos == ydest && listPieces[ i ]->color != piece->color ) {
			tPiece = listPieces[ i ];
			tPiece->inPlay = false;	
			break;
		}
	}

	// move it temporarirly	
	piece->xpos = xdest;
	piece->ypos = ydest;	

	if( KingCheck( piece->color ) )
		checkedSquare = true;

	// restore
	if( tPiece != NULL ) 
		tPiece->inPlay = true;
	piece->xpos = tx;
	piece->ypos = ty;
	
	if( checkedSquare )
		return true;

	return false;	
}

bool Game::MovePiece( Piece_t *piece, const int &xdest, const int &ydest )
{
	bool pawnFlag = false;

	for( int i = 0; i < 32; i++ ) {
		// inplay only
		if( !listPieces[ i ]->inPlay )
			continue;

		// ommit itself
		if( listPieces[ i ]->ID == piece->ID )
			continue;

		// capture state - same color
		if( listPieces[ i ]->xpos == xdest && listPieces[ i ]->ypos == ydest && listPieces[ i ]->color == piece->color ) 
			return false;

		// pawn cannot capture a piece at its own X
		if( listPieces[ i ]->xpos == xdest && listPieces[ i ]->ypos == ydest && ( piece->skinID == BLACK_PAWN || piece->skinID == WHITE_PAWN ) && piece->xpos == xdest )
			return false; 

		// pawn can only capture diagonally	
		if( ( piece->skinID == BLACK_PAWN || piece->skinID == WHITE_PAWN ) && piece->xpos != xdest && listPieces[ i ]->xpos == xdest && listPieces[ i ]->ypos == ydest ) 
			pawnFlag = true; 
	}

	// en passant 
	if( EnPassant( piece, xdest, ydest ) )
		pawnFlag = true;

	// pawn cannot move diagonally	
	if( ( piece->skinID == BLACK_PAWN || piece->skinID == WHITE_PAWN ) && piece->xpos != xdest && !pawnFlag )
		return false; 

	// pawn initial move by 2
	if( ( piece->skinID == BLACK_PAWN && ydest == piece->ypos + 2 ) || ( piece->skinID == WHITE_PAWN && ydest == piece->ypos - 2 ) ) {
		for( int i = 0; i < 32; i++ ) {
			// inplay only
			if( !listPieces[ i ]->inPlay )
				continue;

			// ommit itself
			if( listPieces[ i ]->ID == piece->ID )
				continue;

			// obstruction?
			if( listPieces[ i ]->xpos == xdest && listPieces[ i ]->ypos == ( piece->ypos + 1 ) && piece->skinID == BLACK_PAWN )
				return false;

			// obstruction?
			if( listPieces[ i ]->xpos == xdest && listPieces[ i ]->ypos == ( piece->ypos - 1 ) && piece->skinID == WHITE_PAWN )
				return false;
		}
	}

	if( pawnFlag )
		return pawnFlag;

	return true;
}

void Game::dbg( const char *s )
{
	std::cout << s << std::endl;
}

int Game::MovePieceIter( const int &j, const int &k, const int &xdest, const int &ydest, Piece_t *piece )
{
	for( int i = 0; i < 32; i++ )
	{		
		// there is another piece between origin and destination
		if( listPieces[ i ]->xpos == j && listPieces[ i ]->ypos == k && ( j != xdest || k != ydest ) && piece->ID != listPieces[ i ]->ID && listPieces[ i ]->inPlay )
		{
			printf("obstruction x: %d y: %d\n", j, k );
			return 0; // return false
		}

		// we've reached the destination and there is no piece obstructing the path
		if( j == xdest && k == ydest )
		{
			if( MovePiece( piece, xdest, ydest ) )
				return 1;	// return true
			else
				return 0;	// return false;
		}
	}
	return 2;	// continue
}

bool Game::MovePawn( Piece_t *piece, const int &xdest, const int &ydest )
{
	// white pawn regular move + capture
	if( xdest >= ( piece->xpos - 1 ) && xdest <= ( piece->xpos + 1 ) && ( piece->ypos - 1 ) == ydest && piece->color == COLOR_WHITE )
		return MovePiece( piece, xdest, ydest );

	// white pawn initial move by 2
	if( piece->xpos == xdest && ( piece->ypos - 2 ) == ydest && piece->isInitialState && piece->color == COLOR_WHITE )
		return MovePiece( piece, xdest, ydest );

	// black pawn regular move + capture
	if( xdest >= ( piece->xpos - 1 ) && xdest <= ( piece->xpos + 1 ) && ( piece->ypos + 1 ) == ydest && piece->color == COLOR_BLACK )
		return MovePiece( piece, xdest, ydest );

	// black pawn initial move by 2
	if( piece->xpos == xdest && ( piece->ypos + 2 ) == ydest && piece->isInitialState && piece->color == COLOR_BLACK )
		return MovePiece( piece, xdest, ydest );

	return false;
}

bool Game::MoveQueen( Piece_t *piece, const int &xdest, const int &ydest )
{
	if( ( xdest == piece->xpos && ydest != piece->ypos ) || ( xdest != piece->xpos && ydest == piece->ypos ) )
		return MoveRook( piece, xdest, ydest );
	else
		return MoveBishop( piece, xdest, ydest );

	return false;
}

bool Game::MoveBishop( Piece_t *piece, const int &xdest, const int &ydest )
{
	int j, k;
	j = piece->xpos;
	k = piece->ypos;

	if( piece->xpos < xdest && piece->ypos > ydest )
	{
		for( ; j < 8 && j >= 0 && k < 8 && k >= 0; j++, k-- )
		{
			switch( MovePieceIter( j, k, xdest, ydest, piece ) ) { 
				case 0:	
					return false;
					break;
				case 1:
					return true;				
					break;
				case 2:
					continue;
					break;
			}
		}
	}
	else if( piece->xpos > xdest && piece->ypos > ydest )
	{
		for( ; j < 8 && j >= 0 && k < 8 && k >= 0; j--, k-- )
		{
			switch( MovePieceIter( j, k, xdest, ydest, piece ) ) {
				case 0:	
					return false;
					break;
				case 1:
					return true;				
					break;
				case 2:
					continue;
					break;
			}
		}
	}
	else if( piece->xpos < xdest && piece->ypos < ydest )
	{
		for( ; j < 8 && j >= 0 && k < 8 && k >= 0; j++, k++ )
		{
			switch( MovePieceIter( j, k, xdest, ydest, piece ) ) {
				case 0:	
					return false;
					break;
				case 1:
					return true;				
					break;
				case 2:
					continue;
					break;
			}
		}
	}
	else if( piece->xpos > xdest && piece->ypos < ydest )
	{
		for( ; j < 8 && j >= 0 && k < 8 && k >= 0; j--, k++ )
		{
			switch( MovePieceIter( j, k, xdest, ydest, piece ) ) {
				case 0:	
					return false;
					break;
				case 1:
					return true;				
					break;
				case 2:
					continue;
					break;
			}
		}
	}

	return false;
}

bool Game::MoveRook( Piece_t *piece, const int &xdest, const int &ydest )
{
	int j, k;
	j = piece->xpos;
	k = piece->ypos;

	if( piece->xpos == xdest && piece->ypos > ydest )
	{
		for( ; k < 8 && k >= 0; k-- )
		{
			switch( MovePieceIter( j, k, xdest, ydest, piece ) ) {
				case 0:	
					return false;
					break;
				case 1:
					return true;				
					break;
				case 2:
					continue;
					break;
			}

		}
	}
	else if( piece->xpos == xdest && piece->ypos < ydest )
	{
		for( ; k < 8 && k >= 0; k++ )
		{
			switch( MovePieceIter( j, k, xdest, ydest, piece ) ) {
				case 0:	
					return false;
					break;
				case 1:
					return true;				
					break;
				case 2:
					continue;
					break;
			}

		}
	}
	else if( piece->xpos > xdest && piece->ypos == ydest )
	{
		for( ; j < 8 && j >= 0; j-- )
		{
			switch( MovePieceIter( j, k, xdest, ydest, piece ) ) {
				case 0:	
					return false;
					break;
				case 1:
					return true;				
					break;
				case 2:
					continue;
					break;
			}

		}
	}
	else if( piece->xpos < xdest && piece->ypos == ydest )
	{
		for( ; j < 8 && j >= 0; j++ ) {
			switch( MovePieceIter( j, k, xdest, ydest, piece ) ) {
				case 0:	
					return false;
					break;
				case 1:
					return true;				
					break;
				case 2:
					continue;
					break;
			}
		}
	}

	return false;
}

bool Game::MoveKnight( Piece_t *piece, const int &xdest, const int &ydest )
{
	int *x, *y;
	x = &(piece->xpos);
	y = &(piece->ypos);

	if( *x == ( xdest - 2 ) && *y == ( ydest - 1 ) )
		return MovePiece( piece, xdest, ydest );
	if( *x == ( xdest - 2 ) && *y == ( ydest + 1 ) )
		return MovePiece( piece, xdest, ydest );
	if( *x == ( xdest - 1 ) && *y == ( ydest + 2 ) )
		return MovePiece( piece, xdest, ydest );
	if( *x == ( xdest + 1 ) && *y == ( ydest + 2 ) )
		return MovePiece( piece, xdest, ydest );
	if( *x == ( xdest + 2 ) && *y == ( ydest - 1 ) )
		return MovePiece( piece, xdest, ydest );
	if( *x == ( xdest + 2 ) && *y == ( ydest + 1 ) )
		return MovePiece( piece, xdest, ydest  );
	if( *x == ( xdest + 1 ) && *y == ( ydest - 2 ) )
		return MovePiece( piece, xdest, ydest );
	if( *x == ( xdest - 1 ) && *y == ( ydest - 2 ) )
		return MovePiece( piece, xdest, ydest );

	return false;
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

