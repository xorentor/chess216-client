#ifndef __GFX_H_
#define __GFX_H_

class Image_t
{
public:
	Image_t() {};
	virtual ~Image_t() {};

	SDL_Surface *surface;
	bool inuse;	
};

class GFX
{
public:
	GFX();
	virtual ~GFX();

	bool Init();
	SDL_Rect **GetPieces() { return mappedPieces; };
	void Run( Piece_t **pieces );
	bool SwapBoard( void );
	SDL_Surface *GetScreen() { return screen; }
	std::vector<Image_t *> *GetImages() { return &images; }
	void ApplySurface( const int &x, const int &y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip );  
	bool Flip();
	void SDL_MoveXY( SDL_Surface *srf, int top, int left );

private:
	void MapPieces();
	bool GenerateBoard( const bool reverse );
	void PushImage( SDL_Surface *image );
	void CleanImages();
	void LoadImage( const char *filename );
	void Update( Piece_t **pieces );
	void ClearScreen();
	void PieceDimension( const int &x, const int &y, const int &w, const int &h, SDL_Rect **pieces, const int &piece );

	SDL_Rect **mappedPieces;
	bool fieldSelected;
	std::vector<Image_t *> images;
	SDL_Surface *screen;
	int generatedBoard;
	bool generateReversed;
};

#endif
