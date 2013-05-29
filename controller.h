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
	const int *GetDescriptor() { return socketDesc; }

	// sets
	void SetDescriptor( const int *sd ) { this->socketDesc = sd; }
	void SetMutex( pthread_mutex_t *mt ) { this->mutex = mt; }
	void SetGameListItem( int *gamesListItem ) { this->gamesListItem = gamesListItem; }
	void SetGameCountColumns( int *gamesCountColumns ) { this->gamesCountColumns = gamesCountColumns; }
	void SetGameList( GtkWidget *list ) { this->list = list; }
	void SetSysMsg( GtkWidget *systemMsg ) { this->systemMsg = systemMsg; }
	void SetGameName( GtkWidget *gameName ) { this->buttonGamename = gameName; };
	void SetPlayer1( GtkWidget *player ) { this->buttonPlayer1 = player; };
	void SetPlayer2( GtkWidget *player ) { this->buttonPlayer2 = player; };
	void SetButtonSitActive();

	// gtk
	void GTKAppendGameListItem( const char *str, void *byte );
	void GTKJoinGame( const char *row );
	void GTKSysMsg( const int &code );
	void GTKSetGamename( const char *gameName, void *gameId );
	void GTKSetPlayer1( const char *player );
	void GTKSetPlayer2( const char *player );
	void GTKSetButtonSitActive();
	void GTKRemoveGameListItem( const char *str, void *byte );

private:
	Client client;
	char output[ BUFFER_LEN ];

	// controller
	const int *socketDesc;
	pthread_mutex_t *mutex;
	Game_t games[ MAX_GAMES ];

	// game
	char currentGameId;

	// packets
	PacketData_t pd;
	LoginData_t ld;
	JoinData_t jd;
	GameSitData_t sd;
	MovePieceData_t md;

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
};

#endif
