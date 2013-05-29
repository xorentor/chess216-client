#include <gtk/gtk.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "common.h"
#include "client.h"
#include "controller.h"
#include "sha256.h"

Controller::Controller()
{
	quit = 0;
	memset( games, 0, sizeof( games ) );
}

Controller::~Controller()
{
}

void Controller::ClientLogin( const char *user, const char *pass )
{
	memset( &pd, 0, sizeof( pd ) );
	memset( &ld, 0, sizeof( ld ) );

	if( strlen( user ) < 2 || strlen( pass ) < 2 )
		return;

/*
	command = (char )CMD_LOGIN;
	memset( output, 0, sizeof( output ) );

	memcpy( output, &command, sizeof( command ) );
	memcpy( output + 1, user, strlen( user ) );
	memcpy( output + 33, pass, strlen( pass ) );
*/

        unsigned char hashpass[ 32 ];
        SHA256_CTX ctx;
        sha256_init( &ctx );
        sha256_update( &ctx, (unsigned char *)pass, strlen( pass ) );
        sha256_final( &ctx, hashpass );

	pd.command = (char )CMD_LOGIN;

	pd.data = &ld;
	memcpy( &ld.username, user, strlen( user ) );
	memcpy( &ld.password, hashpass, sizeof( hashpass ) );

	client.Send( socketDesc, &pd );
}

void Controller::ClientGameCreate()
{
	memset( &pd, 0, sizeof( pd ) );

	pd.command = (char )CMD_GAME_CREATE;

	client.Send( socketDesc, &pd );
}

void Controller::GTKAppendGameListItem( const char *str, void *byte )
{
	GtkListStore *store;
	GtkTreeIter iter;

	StoreGame( str, byte );
	
  	store = GTK_LIST_STORE( gtk_tree_view_get_model( GTK_TREE_VIEW( list ) ) );

  	gtk_list_store_append( store, &iter );
  	gtk_list_store_set( store, &iter, *gamesListItem, str, -1 );
}

void Controller::GTKRemoveGameListItem( const char *str, void *byte )
{
	printf( "remove game %s\n", str );
	/*
	GtkTreeModel *model;
	GtkTreeIter iter;

	RemoveGame( str, byte );

	printf( "gamename: %s\n", str );
	model = gtk_tree_view_get_model( (GtkTreeView *)list );
        if( !gtk_tree_model_get_iter_from_string( model, &iter, str ) )
                printf("no game iter\n");


	/*	
  	store = GTK_LIST_STORE( gtk_tree_view_get_model( GTK_TREE_VIEW( list ) ) );
	if( !gtk_tree_model_get_iter_from_string( (GtkTreeModel *)store, &iter, str ) )
		printf("no game iter\n");
  	gtk_list_store_remove( store, &iter );
	*/
  	//gtk_list_store_set( store, &iter, *gamesListItem, str, -1 );
}

void Controller::MovePiece( const int &xsrc, const int &ysrc, const int &xdest, const int &ydest )
{
	memset( &pd, 0, sizeof( pd ) );
	memset( &md, 0, sizeof( md ) );
	pd.data = &md;

	pd.command = (char )CMD_GAME_MOVEPIECE;

	md.xsrc = (char )xsrc;
	md.ysrc = (char )ysrc;
	md.xdest = (char )xdest;
	md.ydest = (char )ydest;

	client.Send( socketDesc, &pd );
}

void Controller::StoreGame( const char *str, void *byte )
{
	for( int i = 0; i < MAX_GAMES; i++ ) {
		if( games[ i ].id == 0 ) {
			games[ i ].id = (int) *(char*)byte;
			memcpy( &games[ i ].value, str, strlen( str ) );
			return;
		}
	}
}

void Controller::RemoveGame( const char *str, void *byte )
{
	for( int i = 0; i < MAX_GAMES; i++ ) {
		if( games[ i ].id > 0 ) {
			if( games[ i ].id == (int ) *(char *)byte ) {
				memset( &games[ i ], 0, sizeof( Game_t ) );
				return;
			}
		}
	}
}

void Controller::ClientPlayerSit( const int &side )
{
	memset( &pd, 0, sizeof( pd ) );
	memset( &sd, 0, sizeof( sd ) );
	pd.data = &sd;

	pd.command = (char )CMD_GAME_SIT;
	sd.gameId = currentGameId;
	sd.slot = (char )side;
	
	client.Send( socketDesc, &pd );
}

void Controller::ClientPlayerStand( const int &side )
{
	memset( &pd, 0, sizeof( pd ) );
	memset( &sd, 0, sizeof( sd ) );
	pd.data = &sd;

	pd.command = (char )CMD_GAME_STAND;
	sd.gameId = currentGameId;
	sd.slot = (char )side;
	
	client.Send( socketDesc, &pd );
}

void Controller::GTKJoinGame( const char *row )
{
	for( int i = 0; i < MAX_GAMES; i++ ) {
		if( games[ i ].id == 0 )
			continue;
		if( strcmp( games[ i ].value, row ) == 0 ) {
			printf("found game %d\n", games[ i ].id );	
			memset( &pd, 0, sizeof( pd ) );
			memset( &jd, 0, sizeof( jd ) );
			pd.data = &jd;

			pd.command = (char )CMD_GAME_JOIN;
			jd.gameId = (char )games[ i ].id;
	
			client.Send( socketDesc, &pd );

			return;
		}
	}
}

void Controller::GTKSetGamename( const char *gameName, void *gameId )
{
	currentGameId = *(char *)gameId;		// copy this
	gtk_button_set_label( (GtkButton *)buttonGamename, gameName );
}

void Controller::GTKSetPlayer1( const char *player )
{
	gtk_button_set_label( (GtkButton *)buttonPlayer1, player );
	//gtk_widget_set_sensitive( buttonPlayer1, FALSE );
}

void Controller::GTKSetPlayer2( const char *player )
{
	gtk_button_set_label( (GtkButton *)buttonPlayer2, player );
	//gtk_widget_set_sensitive( buttonPlayer2, FALSE );
}

void Controller::GTKSetButtonSitActive()
{
	gtk_widget_set_sensitive( buttonPlayer1, TRUE );
	gtk_widget_set_sensitive( buttonPlayer2, TRUE );
}

void Controller::GTKSysMsg( const int &code )
{
	switch( code ) {
		case CMD_LOGIN_PARAM_DETAILS_OK:
			gtk_entry_set_text( (GtkEntry *)systemMsg, "Login successful." );
			break;	
		case CMD_LOGIN_PARAM_DETAILS_ERR:
			gtk_entry_set_text( (GtkEntry *)systemMsg, "Login failed." );
			break;
		case CMD_GAME_CREATE_PARAM_OK:
			gtk_entry_set_text( (GtkEntry *)systemMsg, "Game created successfully." );
			break;
		case CMD_GAME_BEGIN_PARAM_OK:
			gtk_entry_set_text( (GtkEntry *)systemMsg, "Game has started, White player to move." );
			break;
		case CMD_GAME_PARAM_CHECKMATE:
			gtk_entry_set_text( (GtkEntry *)systemMsg, "Check Mate." );
			break;
		case CMD_GAME_PARAM_NEXTWHITE:
			gtk_entry_set_text( (GtkEntry *)systemMsg, "White to move" );
			break;
		case CMD_GAME_PARAM_NEXTBLACK:
			gtk_entry_set_text( (GtkEntry *)systemMsg, "Black to move" );
			break;

	}
}
