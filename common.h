#include <gtk/gtk.h>

#define 	SCREEN_WIDTH 		720
#define 	SCREEN_HEIGHT  		720
#define 	SCREEN_BPP 		32
#define		BOARD_SQUARE_WH		90
#define		DARK_COLOR_R		102
#define		DARK_COLOR_G		102
#define		DARK_COLOR_B		102

#define		GENBOARD_F		0x1
#define		GENBOARDR_F		0x2

#define		COLOR_WHITE		1
#define 	COLOR_BLACK		2

#define 	BUFFER_LEN		256
#define		MAX_GAMES		256

// as same as server ***
enum {
        CMD_LOGIN = 1,                  
        CMD_GAME_CREATE,                
        CMD_GAME_JOIN,                  
	CMD_GAME_SIT,
	CMD_GAME_MOVEPIECE,
};

enum {
        CMD_LOGIN_PARAM_DETAILS_OK = 0,
        CMD_LOGIN_PARAM_DETAILS_ERR,
        CMD_GAME_CREATE_PARAM_OK,
        CMD_GAME_CREATE_PARAM_NOK,
        CMD_GAME_JOIN_PARAM_OK,
        CMD_GAME_JOIN_PARAM_NOK,
	CMD_GAME_BEGIN_PARAM_OK,
	CMD_GAME_PARAM_CHECKMATE,
	CMD_GAME_PARAM_NEXTWHITE,
	CMD_GAME_PARAM_NEXTBLACK,
};

enum
{
	BLACK_KING = 0,
	WHITE_KING,
	BLACK_QUEEN,
	WHITE_QUEEN,	
	BLACK_ROOK,
	WHITE_ROOK,
	BLACK_BISHOP,
	WHITE_BISHOP,
	BLACK_KNIGHT,
	WHITE_KNIGHT,
	BLACK_PAWN,
	WHITE_PAWN
};

// ***  

extern GtkWidget *text_login, *text_pass;
extern GtkWidget *listGames, *listSpectators;
extern GtkTreeSelection *selection;
extern GtkWidget *systemMsg, *buttonGamename;
extern GtkWidget *buttonPlayer1, *buttonPlayer2;
extern int quit;

typedef struct Game_s
{
	int id;
	const char *value;
} Game_t;

typedef struct LoginData_s
{
        char username[ 32 ];
        char password[ 32 ];
} LoginData_t;

typedef struct MovePieceData_s
{
	char xsrc;
	char ysrc;
	char xdest;
	char ydest;
} MovePieceData_t;

typedef struct GamePieceMoveSrv_s
{
        char pieceId;
        char xdest;
        char ydest;
	char checkMate;
	char next;
} GamePieceMoveSrv_t;

typedef struct JoinData_s
{
	char gameId;
} JoinData_t;

typedef struct GameSitData_s
{
	char gameId;
	char slot;
} GameSitData_t;

typedef struct GameSitServerData_s
{
	char slot;
	char username[ 32 ];
	char gameBegin;
} GameSitServerData_t;

typedef struct PacketData_s
{
        char command;
	char length;
        void *data;
} PacketData_t;

typedef struct ServerByte_s
{
	char byte;
} ServerByte_t;

typedef struct ServerTwoBytes_s
{
        char byte0;
        char byte1;
} ServerTwoBytes_t;
