#include "common.h"
#include "client.h"

Client::Client()
{
}

Client::~Client()
{
}

#define CPYDTA( T )\
    memcpy( output + initLen, pd->data, sizeof( T ) );
int Client::Send( const int sd, struct packetdata_s *pd )
{
	int n;
	int initLen;
	char output[ BUFFER_LEN ];

	memset( output, 0, BUFFER_LEN );
	initLen = sizeof( pd->command ) + sizeof( pd->length );
	memcpy( output, &pd->command, sizeof( pd->command ) );

	switch( (int )pd->command ) {
		case CMD_LOGIN:
            CPYDTA( struct logindata_s );
			break;
		case CMD_GAME_CREATE:
			// no data			
			break;
		case CMD_GAME_JOIN:
			CPYDTA( struct joindata_s );
			break;
		case CMD_GAME_SIT:
			CPYDTA( struct gamesitdata_s );
			break;
		case CMD_GAME_MOVEPIECE:
			CPYDTA( struct movepiecedata_s );
			break;
		case CMD_GAME_STAND:
			CPYDTA( struct gamestanddata_s );
			break;
	}

	n = write( sd, output, BUFFER_LEN );
    LM_INFO( "Bytes sent %d\n", n );

    return 0;
}

int Client::Receive()
{
    return 0;
}
