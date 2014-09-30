#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_


class Controller 
{
public:
	Controller();
	virtual ~Controller();

	// controller	
	void ClientLogin( const char *user, const char *pass );
	void ClientGameCreate();
	void ClientPlayerSit( const int &side );
	void ClientPlayerStand( const int &side );
	void MovePiece( const int &xsrc, const int &ysrc, const int &xdest, const int &ydest );

	int Stop() { return quit; }
	void SetStop() { quit = 1; }
	const int GetDescriptor() { return socketDesc; }

	// sets
	void SetDescriptor( const int sd ) { this->socketDesc = sd; }
	void SetMutex( pthread_mutex_t *mt ) { this->mutex = mt; }
	void SetGameListItem( int *gamesListItem ) { this->gamesListItem = gamesListItem; }
	void SetGameCountColumns( int *gamesCountColumns ) { this->gamesCountColumns = gamesCountColumns; }
	void SetGameList( GtkWidget *list ) { this->list = list; }
	void SetSysMsg( GtkWidget *systemMsg ) { this->systemMsg = systemMsg; }
	void SetGameName( GtkWidget *gameName ) { this->buttonGamename = gameName; };
	void SetPlayer1( GtkWidget *player ) { this->buttonPlayer1 = player; };
	void SetPlayer2( GtkWidget *player ) { this->buttonPlayer2 = player; };
	void SetLoggedMsg( GtkWidget *loggedMsg ) { this->loggedMsg = loggedMsg; };
	void SetEloMsg( GtkWidget *eloMsg ) { this->eloMsg = eloMsg; };
	void SetLoginWindow( GtkWidget *loginWindow ) { this->loginWindow = loginWindow; };
	void SetTimers( GtkWidget *timerP1, GtkWidget *timerP2 ) { this->timerP1 = timerP1; this->timerP2 = timerP2; };
	void SetButtonSitActive();

	// gtk
	void GTKAppendGameListItem( const char *str, void *byte );
	void GTKJoinGame( const char *row );
	void GTKSysMsg( const int &code );
	void GTKLoggedUser( const char *str );
	void GTKSetGamename( const char *gameName, void *gameId, bool finished );
	void GTKSetPlayer1( const char *player );
	void GTKSetPlayer2( const char *player );
	void GTKSetButtonSitActive();
	void GTKSetButtonSitInActive();
	void GTKRemoveGameListItem( const char *str, void *byte );
	void GTKSetElo( const double elo );
	void GTKSetTimer( const char p1min, const char p1sec, const char p2min, const char p2sec );
	void GTKHideLogin();

private:
	Client client;
	char output[ BUFFER_LEN ];

	// controller
	int socketDesc;
	pthread_mutex_t *mutex;
	struct game_s games[ MAX_GAMES ];

	// game
	char currentGameId;

	// packets
	struct packetdata_s pd;
	struct logindata_s ld;
	struct joindata_s jd;
	struct gamesitdata_s sd;
	struct movepiecedata_s md;

	void StoreGame( const char *str, void *byte );
	void RemoveGame( const char *str, void *byte );

	// gtk
	int *gamesListItem;
	int *gamesCountColumns;
	GtkWidget *list;
	GtkWidget *systemMsg;
	GtkWidget *buttonGamename;
	GtkWidget *buttonPlayer1;
	GtkWidget *buttonPlayer2;
	GtkWidget *loggedMsg;
	GtkWidget *eloMsg;
	GtkWidget *loginWindow;
	GtkWidget *timerP1;
	GtkWidget *timerP2;
};

#endif
