#ifndef __CLIENT_H_
#define __CLIENT_H_

class Client
{
public:
	Client();
	virtual ~Client();

	int Send( const int *sd, PacketData_t *pd );
	int Receive();
};

#endif
