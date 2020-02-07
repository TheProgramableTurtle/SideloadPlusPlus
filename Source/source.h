#pragma once

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <string.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

const int g_DEF_BUFLEN = 500;

class Client {
public:
	Client(SOCKET*, SOCKET*);
	~Client();

	void carrRet();

	int acceptConn();

	int recvData(SOCKET*);

	int checkMsg(int);
	int sendData(char*, char*, char*, char*, char*);

	SOCKET *ClientSocket;
	SOCKET *ListenSocket;

	char recvBuf[g_DEF_BUFLEN];
	int recvLen;
	char sendBuf[g_DEF_BUFLEN];
	int sendLen;
	int iSendResult;

};


void HangCMD();

void callNull();

int callWinsockError(int errorCode, const char *errorType, bool getError, int WSAAPI cleanupFunc(), struct addrinfo *addrToFree, SOCKET *socketToClose);

int checkWinsockError(int errorCode, const char *errorType, bool getError, int WSAAPI cleanupFunc(), struct addrinfo *addrToFree, SOCKET *socketToClose, bool sockError);
