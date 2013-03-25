#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <memory.h>

#include "common.h"
#include "client.h"

Client::Client()
{
}

Client::~Client()
{
}

int Client::Send( const int *sd, PacketData_t *pd )
{
	int n;
	int initLen;
	char output[ BUFFER_LEN ];

	memset( output, 0, BUFFER_LEN );
	initLen = sizeof( pd->command ) + sizeof( pd->length );
	memcpy( output, &pd->command, sizeof( pd->command ) );

	switch( (int )pd->command ) {
		case CMD_LOGIN:
			// length
			// TODO: check this on server side
			memset( output + sizeof( pd->command ),  (char )( sizeof( ( (LoginData_t *)pd->data )->username ) + sizeof( ( (LoginData_t *)pd->data )->password ) ), sizeof( pd->length ) );
			// data
			memcpy( output + initLen, &( (LoginData_t *)pd->data )->username, strlen( ( (LoginData_t *)pd->data )->username ) );
			memcpy( output + initLen + sizeof( ( (LoginData_t *)pd->data )->username ), &( (LoginData_t *)pd->data )->password, strlen( ( (LoginData_t *)pd->data )->password ) );
			break;
		case CMD_GAME_CREATE:
			// no data			
			break;
		case CMD_GAME_JOIN:
			memcpy( output + initLen, &( (JoinData_t *)pd->data )->gameId, sizeof( ( (JoinData_t *)pd->data )->gameId ) );
			break;
		case CMD_GAME_SIT:
			memcpy( output + initLen, &( (GameSitData_t *)pd->data )->gameId, sizeof( ( (GameSitData_t *)pd->data )->gameId ) );
			memcpy( output + initLen + sizeof( ( (GameSitData_t *)pd->data )->gameId ), &( (GameSitData_t *)pd->data )->slot, sizeof( ( (GameSitData_t *)pd->data )->slot ) );
			break;
		case CMD_GAME_MOVEPIECE:
			memcpy( output + initLen, &( (MovePieceData_t *)pd->data )->xsrc, sizeof( ( (MovePieceData_t *)pd->data )->xsrc ) );
			memcpy( output + initLen + 1, &( (MovePieceData_t *)pd->data )->ysrc, sizeof( ( (MovePieceData_t *)pd->data )->ysrc ) );
			memcpy( output + initLen + 2, &( (MovePieceData_t *)pd->data )->xdest, sizeof( ( (MovePieceData_t *)pd->data )->xdest ) );
			memcpy( output + initLen + 3, &( (MovePieceData_t *)pd->data )->ydest, sizeof( ( (MovePieceData_t *)pd->data )->ydest ) );
			break;
	}

	n = write( *sd, output, BUFFER_LEN );
}

int Client::Receive()
{
}
