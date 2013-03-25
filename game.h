#ifndef __GAME_H_
#define __GAME_H_


typedef struct Move_s
{
	char skinID;
	char srcX;
	char srcY;
	char destX;
	char destY;
} Move_t;

class Piece_t
{
public:
	Piece_t();
	virtual ~Piece_t();
	
	SDL_Rect *skin;
	int x;
	int y;
	int xpos;
	int ypos;
	bool inPlay;
	bool isSelected;
	int ID;
	int skinID;
	int color;
	bool isInitialState;
};

class Game
{
public:
	Game();
	virtual ~Game();
	Piece_t **GetPieces() { return listPieces; };
	void MouseInput( const int &x, const int &y );
	const bool Init( SDL_Rect**, Controller *controller );	
	void FinalMovePiece( const int &p, const int &x, const int &y );
private:
	bool CheckMove( Piece_t *piece, const int &xdest, const int &ydest );
	void AddPiece( Piece_t **listPieces, const int &i, const int &x, const int &y, const int &skin, const int &color );
	bool MoveKnight( Piece_t *piece, const int &xdest, const int &ydest );
	bool MoveBishop( Piece_t *piece, const int &xdest, const int &ydest );
	bool MoveRook( Piece_t *piece, const int &xdest, const int &ydest );
	bool MoveQueen( Piece_t *piece, const int &xdest, const int &ydest );
	bool MoveKing( Piece_t *piece, const int &xdest, const int &ydest );
	bool MovePawn( Piece_t *piece, const int &xdest, const int &ydest );

	bool KingCheck( const int &color );
	bool KingCheckSimulate( Piece_t *piece, const int &xdest, const int &ydest );
	bool KingCheckMate( const int &color );
	bool SquareChecked( const int &x, const int &y, const int &color );
	bool SquareFree( const int &x, const int &y );
	bool EnPassant( Piece_t *piece, const int &xdest, const int &ydest );

	bool MovePiece( Piece_t *piece, const int &xdest, const int &ydest );
	int MovePieceIter( const int &j, const int &k, const int &xdest, const int &ydest, Piece_t *piece );
	bool CheckSpecials( Piece_t *piece, const int &xdest, const int &ydest );

	void dbg( const char *s );

	Move_t lastMove;

	Controller *controller;	
	Piece_t **listPieces;
	SDL_Rect **skins;
	int *sd;
	Piece_t *simulatePiece;
};

#endif
