#ifndef __CLIENT_H_
#define __CLIENT_H_

class Client
{
public:
	Client();
	virtual ~Client();

	int Send( const int sd, struct packetdata_s *pd );
	int Receive();
};

#endif
