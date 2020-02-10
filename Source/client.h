#pragma once

#include "sideload.h"
#include "errors.h"
#include "globals.h"

class Client {
public:
	Client(SOCKET*, SOCKET*);
	~Client();

	void carrRet();

	int acceptConn();

	int recvData(SOCKET*);

	int checkMsg(int);
	int sendData(char*, char*, char*, char*, char*);

	int setClient(SOCKET);
	int setListen(SOCKET*);

	SOCKET *ClientSocket;
	SOCKET *ListenSocket;

	char recvBuf[g_DEF_BUFLEN];
	int recvLen;
	char sendBuf[g_DEF_BUFLEN];
	int sendLen;
	int iSendResult;

};