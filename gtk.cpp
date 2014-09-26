#include <gtk/gtk.h>
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <SDL/SDL_image.h>

#include "common.h"
#include "client.h"
#include "controller.h"
#include "gtk.h"
#include "sha256.h"

// these must stand out of class, 
// because GTK cannot access them as class members through static cast ( or can it? )
// the solution would be to use gtkmm
GtkWidget *text_login, *text_pass;
GtkWidget *listGames, *listSpectators;
GtkTreeSelection *selection;
GtkWidget *systemMsg, *buttonGamename, *loggedMsg, *eloMsg, *timerP1, *timerP2;
GtkWidget *buttonPlayer1, *buttonPlayer2;
GtkWidget *gameWindow, *loginWindow;

int gamesListItem;
int gamesCountColumns;

GTK::GTK()
{
	destPtr = &GTK::Destroy;
	loginPtr = &GTK::Login;
	gamesListItem = 0;
	gamesCountColumns = 1;
	createGamePtr = &GTK::CreateGame;
	joinGamePtr = &GTK::JoinGame;
}

GTK::~GTK()
{
}

void GTK::Destroy( void *gtk ) 
{
	gtk_widget_destroy( loginWindow );
	gtk_widget_destroy( gameWindow );
	SDL_Quit();
	gtk_main_quit();
	exit( 0 );
}

void GTK::Player1Sit( void *gtk )
{
	const gchar *text = gtk_button_get_label( (GtkButton *)buttonPlayer1 );
	if( strcmp( text, "Sit" ) == 0 ) 
		GTK::controller->ClientPlayerSit( COLOR_WHITE );	
	else
		GTK::controller->ClientPlayerStand( COLOR_WHITE );	
}

void GTK::Player2Sit( void *gtk )
{
	const gchar *text = gtk_button_get_label( (GtkButton *)buttonPlayer2 );
	if( strcmp( text, "Sit" ) == 0 )
		GTK::controller->ClientPlayerSit( COLOR_BLACK );	
	else
		GTK::controller->ClientPlayerStand( COLOR_BLACK );	
}

void GTK::Login( void *gtk ) {
	const char *login, *pass;

	login = gtk_entry_get_text( (GtkEntry *)text_login );
	pass = gtk_entry_get_text( (GtkEntry *)text_pass );

	GTK::controller->ClientLogin( login, pass );
}

void GTK::CreateGame( void *gtk )
{
	GTK::controller->ClientGameCreate();
}

void GTK::JoinGame( void *gtk )
{
  	GtkListStore *store;
  	GtkTreeModel *model;
  	GtkTreeIter  iter;
      	char *row;

  	store = GTK_LIST_STORE( gtk_tree_view_get_model( GTK_TREE_VIEW( listGames ) ) );
  	model = gtk_tree_view_get_model( GTK_TREE_VIEW( listGames ) );

  	if( gtk_tree_model_get_iter_first( model, &iter ) == FALSE )
      		return;

	if ( gtk_tree_selection_get_selected( GTK_TREE_SELECTION( selection ), &model, &iter ) ) {
      		gtk_tree_model_get( model, &iter, gamesListItem, &row,  -1 );
		GTK::controller->GTKJoinGame( row );
	    	//gtk_list_store_remove(store, &iter);
  	}
}

void GTK::GameMenu()
{
  	GtkWidget *sw;

  	GtkWidget *buttonJoin;
  	GtkWidget *buttonCreate;
//  	GtkWidget *removeAll;

  	GtkWidget *vbox;
  	GtkWidget *hbox, *hbox1, *hboxp1, *hboxp2, *hboxGamename;
	GtkWidget *vboxGames;

  	GtkWidget *separator;
  	GtkWidget *labelPlayer1, *labelPlayer2, *labelSpectators, *labelGamename;
	GtkWidget *labelPlayer, *labelElo;

  	gameWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  	sw = gtk_scrolled_window_new( NULL, NULL );
  	listGames = gtk_tree_view_new();
  	//listSpectators = gtk_tree_view_new();

  	gtk_window_set_title (GTK_WINDOW (gameWindow), "Game Menu");
  	gtk_window_set_position(GTK_WINDOW(gameWindow), GTK_WIN_POS_CENTER);
  	gtk_container_set_border_width (GTK_CONTAINER (gameWindow), 10);
  	gtk_widget_set_size_request( gameWindow, 370, 720 );
	gtk_window_move( (GtkWindow *)gameWindow, 800, 0 ); 

  	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_ETCHED_IN);

  	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (listGames), FALSE);
//  	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (listSpectators), FALSE);

  	vbox = gtk_vbox_new( FALSE, 0 );

  	hbox = gtk_hbox_new( TRUE, 5 );
  	hbox1 = gtk_hbox_new( TRUE, 5 );
  	hboxp1 = gtk_hbox_new( TRUE, 5 );
  	hboxp2 = gtk_hbox_new( TRUE, 5 );
	vboxGames = gtk_vbox_new( TRUE, 5 );
  	hboxGamename = gtk_hbox_new( TRUE, 5 );

	loggedMsg = gtk_entry_new();
	eloMsg = gtk_entry_new();

	labelPlayer = gtk_label_new( "Player" );
  	gtk_box_pack_start(GTK_BOX(hbox1), labelPlayer, FALSE, TRUE, 3);
	gtk_box_pack_start(GTK_BOX(hbox1), loggedMsg, FALSE, TRUE, 3);
	labelElo = gtk_label_new( "Elo" );
  	gtk_box_pack_start(GTK_BOX(hbox1), labelElo, FALSE, TRUE, 3);
  	gtk_box_pack_start(GTK_BOX(hbox1), eloMsg, FALSE, TRUE, 3);
  	gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, TRUE, 3);

  	gtk_box_pack_start(GTK_BOX(vbox), sw, TRUE, TRUE, 0);

  	buttonCreate = gtk_button_new_with_label("New Game");
  	buttonJoin = gtk_button_new_with_label("Join Game");
//  	removeAll = gtk_button_new_with_label("Remove All");
 	systemMsg = gtk_entry_new();

  	gtk_box_pack_start(GTK_BOX(hbox), buttonCreate, FALSE, TRUE, 3);
  	//gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, TRUE, 3);
  	gtk_box_pack_start(GTK_BOX(hbox), buttonJoin, FALSE, TRUE, 3);
  	//gtk_box_pack_start(GTK_BOX(hbox), removeAll, FALSE, TRUE, 3);
  	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 3);
	gtk_box_pack_start(GTK_BOX(vbox), vboxGames, FALSE, TRUE, 3);

  	gtk_container_add(GTK_CONTAINER (sw), listGames);
  	gtk_container_add(GTK_CONTAINER (gameWindow), vbox);

  	GameMenuList();
	
	separator = gtk_hseparator_new();
  	gtk_box_pack_start(GTK_BOX(vboxGames), separator, FALSE, TRUE, 3);

	gtk_entry_set_editable( (GtkEntry *)eloMsg, FALSE );
	gtk_entry_set_editable( (GtkEntry *)loggedMsg, FALSE );

	labelGamename = gtk_label_new( "Gamename:" );
  	gtk_box_pack_start(GTK_BOX(hboxGamename), labelGamename, FALSE, TRUE, 3);
//    	gtk_box_pack_start (GTK_BOX (label1), password, TRUE, TRUE, 0);
//	gtk_widget_show( label2 );
  	buttonGamename = gtk_label_new( "No Game" );
  	gtk_box_pack_start(GTK_BOX(hboxGamename), buttonGamename, FALSE, TRUE, 3);
//	gtk_widget_set_sensitive( buttonGamename, FALSE );
  	gtk_box_pack_start(GTK_BOX(vboxGames), hboxGamename, FALSE, TRUE, 3);


	labelPlayer1 = gtk_label_new( "White" );
  	gtk_box_pack_start(GTK_BOX(vboxGames), labelPlayer1, FALSE, TRUE, 3);

  	buttonPlayer1 = gtk_button_new_with_label( "Sit" );
  	gtk_box_pack_start(GTK_BOX(hboxp1), buttonPlayer1, FALSE, TRUE, 3);
	gtk_widget_set_sensitive( buttonPlayer1, FALSE );
 	timerP1 = gtk_label_new("0");
  	gtk_box_pack_start(GTK_BOX(hboxp1), timerP1, FALSE, TRUE, 3);
  	gtk_box_pack_start(GTK_BOX(vboxGames), hboxp1, FALSE, TRUE, 3);

	labelPlayer2 = gtk_label_new( "Black" );
  	gtk_box_pack_start(GTK_BOX(vboxGames), labelPlayer2, FALSE, TRUE, 3);

  	buttonPlayer2 = gtk_button_new_with_label( "Sit" );
  	gtk_box_pack_start(GTK_BOX(hboxp2), buttonPlayer2, FALSE, TRUE, 3);
	gtk_widget_set_sensitive( buttonPlayer2, FALSE );
 	timerP2 = gtk_label_new("0");
  	gtk_box_pack_start(GTK_BOX(hboxp2), timerP2, FALSE, TRUE, 3);
  	gtk_box_pack_start(GTK_BOX(vboxGames), hboxp2, FALSE, TRUE, 3);

	//labelSpectators = gtk_label_new( "Spectators:" );
  	//gtk_box_pack_start( GTK_BOX(vboxGames), labelSpectators, FALSE, TRUE, 3 );
  	gtk_box_pack_start(GTK_BOX(vboxGames), systemMsg, FALSE, TRUE, 3);
	gtk_entry_set_editable( (GtkEntry *)systemMsg, FALSE );
//  	gtk_box_pack_start( GTK_BOX(vboxGames), listSpectators, FALSE, TRUE, 3);
	//qbutton = gtk_button_new_with_label("Quit");
	//gtk_container_add (GTK_CONTAINER (vbox), qbutton );

	selection  = gtk_tree_view_get_selection( GTK_TREE_VIEW( listGames ) );

	gtk_signal_connect_object (GTK_OBJECT (buttonCreate), "clicked", GTK_SIGNAL_FUNC ( createGamePtr ), this);
	gtk_signal_connect_object (GTK_OBJECT (buttonJoin), "clicked", GTK_SIGNAL_FUNC ( joinGamePtr ), this );
	gtk_signal_connect_object (GTK_OBJECT (buttonPlayer1), "clicked", GTK_SIGNAL_FUNC ( &GTK::Player1Sit ), this );
	gtk_signal_connect_object (GTK_OBJECT (buttonPlayer2), "clicked", GTK_SIGNAL_FUNC ( &GTK::Player2Sit ), this );
	gtk_signal_connect( GTK_OBJECT ( gameWindow ), "destroy", GTK_SIGNAL_FUNC( destPtr ), this );

  	gtk_widget_show_all( gameWindow );
}

void GTK::GameMenuList()
{
	GtkCellRenderer    *renderer;
  	GtkTreeViewColumn  *column;
  	GtkListStore       *store;

  	renderer = gtk_cell_renderer_text_new();
  	column = gtk_tree_view_column_new_with_attributes( "List Item", renderer, "text", gamesListItem, NULL );
  	gtk_tree_view_append_column( GTK_TREE_VIEW( listGames ), column );

  	store = gtk_list_store_new( gamesCountColumns, G_TYPE_STRING );

  	gtk_tree_view_set_model( GTK_TREE_VIEW( listGames ), GTK_TREE_MODEL( store ) );

  	g_object_unref( store );
}

void GTK::LoginWindow()
{
	GtkWidget *vbox;
	GtkWidget *labelLogin, *labelPass;
    	GtkWidget *loginbutton;

	loginWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (loginWindow), vbox);
	gtk_widget_show( vbox);

	gtk_window_set_default_size( (GtkWindow *) loginWindow, 100, 200 );
	gtk_window_move( (GtkWindow *) loginWindow, 500, 0 ); 

  	gtk_window_set_title (GTK_WINDOW (loginWindow), "Login");

	labelLogin = gtk_label_new ("Login:");
	gtk_container_add (GTK_CONTAINER (vbox), labelLogin);
	gtk_widget_show( labelLogin );

	text_login = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (text_login), 32);
	gtk_editable_select_region (GTK_EDITABLE (text_login), 0, GTK_ENTRY (text_login)->text_length);
	gtk_box_pack_start (GTK_BOX (vbox), text_login, TRUE, TRUE, 0);
	gtk_widget_show (text_login);
	gtk_widget_show( text_login );

	labelPass = gtk_label_new ("Password:");
	gtk_container_add (GTK_CONTAINER (vbox), labelPass);
	gtk_widget_show( labelPass );

	text_pass = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (text_pass), 32);
	gtk_editable_select_region (GTK_EDITABLE (text_pass), 0, GTK_ENTRY (text_pass)->text_length);
	gtk_box_pack_start (GTK_BOX (vbox), text_pass, TRUE, TRUE, 0);
	gtk_widget_show( text_pass );

	loginbutton = gtk_button_new_with_label("Submit");
	gtk_box_pack_start (GTK_BOX (vbox), loginbutton, TRUE, FALSE, 0);
	gtk_widget_show( loginbutton );
	
	gtk_signal_connect( GTK_OBJECT ( loginWindow ), "destroy", GTK_SIGNAL_FUNC( destPtr ), this );
	gtk_signal_connect_object (GTK_OBJECT (loginbutton ), "clicked", GTK_SIGNAL_FUNC ( loginPtr ), this );
	gtk_widget_show( loginWindow );
}

void GTK::Init( Controller *controller ) 
{
	gtk_init( NULL, NULL );

	this->controller = controller;
	GameMenu();
	LoginWindow();

	controller->SetGameList( listGames );
	controller->SetSysMsg( systemMsg );
	controller->SetGameListItem( &gamesListItem );
	controller->SetGameCountColumns( &gamesCountColumns );
	controller->SetGameName( buttonGamename );
	controller->SetPlayer1( buttonPlayer1 );
	controller->SetPlayer2( buttonPlayer2 );
	controller->SetLoggedMsg( loggedMsg );
	controller->SetEloMsg( eloMsg );
	controller->SetLoginWindow( loginWindow );
	controller->SetTimers( timerP1, timerP2 );
}
