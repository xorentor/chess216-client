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
	void SrvInitPieces( char *data );
	void FinalMovePiece( const int &p, const int &x, const int &y );
private:
	void AddPiece( Piece_t **listPieces, const int &i, const int &x, const int &y, const int &skin, const int &color );
	bool EnPassant( Piece_t *piece, const int &xdest, const int &ydest );

	Move_t lastMove;
	Controller *controller;	
	Piece_t **listPieces;
	SDL_Rect **skins;
	Piece_t *simulatePiece;
};

#endif
