#ifndef __GTK_H_
#define __GTK_H_

class GTK
{
public:
	GTK();
	virtual ~GTK();
	
	void Init( Controller *controller );

private:
	void LoginWindow();
	void GameMenuList();
	void GameMenu();
	void Login( void* );
	void Destroy( void* );
	void CreateGame( void* );
	void JoinGame( void* );
	void Player1Sit( void* );
	void Player2Sit( void* );

	void (GTK::*destPtr)( void* );
	void (GTK::*loginPtr)( void* );
	void (GTK::*createGamePtr)( void* );
	void (GTK::*joinGamePtr)( void* );
	Controller *controller;
};

#endif
