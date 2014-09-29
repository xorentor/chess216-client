#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdarg.h>
#include <syslog.h>

#include <gtk/gtk.h>

#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"
#include <SDL/SDL_image.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define 	SCREEN_WIDTH 		720
#define 	SCREEN_HEIGHT  		720
#define 	SCREEN_BPP 		    32
#define		BOARD_SQUARE_WH		90
#define		DARK_COLOR_R		102
#define		DARK_COLOR_G		102
#define		DARK_COLOR_B		102

#define		GENBOARD_F		    0x1
#define		GENBOARDR_F		    0x2

#define		COLOR_WHITE		1
#define 	COLOR_BLACK		2

#define 	BUFFER_LEN		256
#define		MAX_GAMES		256

#define DP_PREFIX
#define DP_INFO_TEXT        "INFO: "
#define DP_ERROR_TEXT       "ERROR: "
#define DP_WARNING_TEXT     "WARNING: "

#define MY_DPRINT( ...) \
	dprint( __VA_ARGS__ ) \

#define DP_INFO_PREFIX   DP_PREFIX DP_INFO_TEXT
#define DP_WARNING_PREFIX   DP_PREFIX DP_WARNING_TEXT
#define DP_ERROR_PREFIX   DP_PREFIX DP_ERROR_TEXT

#define LM_INFO( ...) \
	MY_DPRINT( DP_INFO_PREFIX __VA_ARGS__);

#define LM_ERR( ...) \
	MY_DPRINT( DP_ERROR_PREFIX __VA_ARGS__ );\
    exit(0);

#define LM_WARN( ...) \
	MY_DPRINT( DP_WARNING_PREFIX __VA_ARGS__);

// as same as server ***
enum {
    CMD_LOGIN = 1,
    CMD_GAME_CREATE,
    CMD_GAME_JOIN,
    CMD_GAME_SIT,
    CMD_GAME_MOVEPIECE,
    CMD_GAME_INITIAL_PIECES,
    CMD_GAME_STAND,
    CMD_GAME_TIMER,
	CMD_GAME_FINISHED,
	CMD_GAME_ELO
};

enum {                  
    CMD_LOGIN_PARAM_DETAILS_OK = 0,
    CMD_LOGIN_PARAM_DETAILS_ERR,
    CMD_GAME_CREATE_PARAM_OK,
    CMD_GAME_CREATE_PARAM_NOK,
    CMD_GAME_JOIN_PARAM_OK,
    CMD_GAME_JOIN_PARAM_NOK,
    CMD_GAME_BEGIN_PARAM_OK,
    CMD_GAME_PARAM_NEXTWHITE,
    CMD_GAME_PARAM_NEXTBLACK,
    CMD_GAME_CREATE_PARAM_DELETE,
    CMD_GAME_STAND_PARAM_OK,
    CMD_GAME_STAND_PARAM_NOK,
    CMD_GAME_PARAM_CHECKMATE_W, 
    CMD_GAME_PARAM_CHECKMATE_B,
    CMD_GAME_TIMER_PARAM_W, 
    CMD_GAME_TIMER_PARAM_B,
	CMD_GAME_FINISHED_DRAW
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

extern GtkWidget *text_login, *text_pass;
extern GtkWidget *listGames, *listSpectators;
extern GtkTreeSelection *selection;
extern GtkWidget *systemMsg, *buttonGamename;
extern GtkWidget *buttonPlayer1, *buttonPlayer2;
extern int quit;

struct game_s {
	int id;
	char value[ 32 ];
};

struct logindata_s {
    char username[ 32 ];
    char password[ 32 ];
};

typedef struct MovePieceData_s {
	char xsrc;
	char ysrc;
	char xdest;
	char ydest;
} MovePieceData_t;

typedef struct GamePieceMoveSrv_s {
    char pieceId;
    char xdest;
    char ydest;
	char checkMate;
	char next;
} GamePieceMoveSrv_t;

typedef struct JoinData_s {
	char gameId;
} JoinData_t;

typedef struct GameSitData_s {
	char gameId;
	char slot;
} GameSitData_t;

typedef struct GameSitServerData_s {
	char slot;
	char username[ 32 ];
	char gameBegin;
} GameSitServerData_t;

typedef struct GameStandServerData_s {
    char param;
    char slot;
} GameStandServerData_t;

typedef struct GameStandData_s {
    char gameId;
    char slot;
} GameStandData_t;

typedef struct PacketData_s {
    char command;
    char length;
    void *data;
} PacketData_t;

typedef struct GameTimerSrv_s {
    char p1_min;
    char p1_sec;
    char p2_min;
    char p2_sec;
} GameTimerSrv_t;

typedef struct GameLoginSrv_s {   
    char param;
    char username[ 32 ];
    double elorating;
} GameLoginSrv_t;

typedef struct EloSrv_s {
    int elo_value;
} EloSrv_t;

extern void dprint( const char * format, ... );
