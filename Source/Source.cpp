#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "source.h"
#include "globals.h"

char g_recvbuf[g_DEF_BUFLEN];


struct ResponseValues {
	char statusCode[20];
	char contentType[30];
	char connStatus[20];
	char msgBody[100];
	char msgLen[5];
	ResponseValues(const char *statusCode, const char *contentType, const char *connStatus, const char *msgBody, const char *msgLen) {
		strcpy(this->statusCode, statusCode);
		strcpy(this->contentType, contentType);
		strcpy(this->connStatus, connStatus);
		strcpy(this->msgBody, msgBody);
		strcpy(this->msgLen, msgLen);
	}
};


int main() {
	signal(SIGINT, exceptionHandler);

	WSADATA wsaData;

	int iResult;
	
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (checkWinsockError(iResult, "WSA Startup error")) return 1;

	const char DEF_PORT[] = "8888";

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEF_PORT, &hints, &result);
	if (checkWinsockError(iResult, "getaddrinfo error", false, WSACleanup)) return 1;

	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (checkWinsockError(ListenSocket, "Socket Error", true, WSACleanup, result, nullptr, true)) return 1;

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	
	if (checkWinsockError(iResult, "Bind Error", true, WSACleanup, result, &ListenSocket)) return 1;


	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		callWinsockError(0, "Listen Error", true, WSACleanup, nullptr, &ListenSocket);
		return 1;
	}

	ResponseValues values("200 OK", "text/html", "Closed", "<html>\r\n<head>\r\n</head>\r\n<body>\r\n<h1>Carpe Diem!:(</h1>\r\n</body>\r\n</html>", "82");

	static SOCKET *ClientSocket = new SOCKET;
	*ClientSocket = INVALID_SOCKET;
	printf("Socket on initialisation: %d\n", ClientSocket);
	Client* iPhone = new Client(ClientSocket, &ListenSocket);
	while (!g_exit) {
		printf("Socket before accept: %d\n", ClientSocket);
		printf("Socket pointer before accept: %d\n", *ClientSocket);
		if (iPhone->acceptConn()) return 1;

		printf("Socket after accept: %d\n", ClientSocket);
		// printf("Socket pointer after accept: %d\n", *ClientSocket);
		printf("Socket reference after accept: %d\n", &ClientSocket);

		iResult = iPhone->recvData(ClientSocket);
		printf("error: %d\n", WSAGetLastError());

		while (iPhone->checkMsg(iResult) == 0) {
			iPhone->sendData(values.statusCode, values.contentType, values.connStatus, values.msgBody, values.msgLen);
			iResult = iPhone->recvData(ClientSocket);
		}
		printf("\nerror: %d\n", WSAGetLastError());

		printf("Socket before shutdown: %d\n", ClientSocket);

		iResult = shutdown(*ClientSocket, SD_SEND);
		if (checkWinsockError(iResult, "Shutdown Error", true, WSACleanup, nullptr, ClientSocket)) return 1;

		closesocket(*ClientSocket);

		if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
			callWinsockError(0, "Listen Error", true, WSACleanup, nullptr, &ListenSocket);
			return 1;
		}
		/*
		ClientSocket = new SOCKET;
		*ClientSocket = INVALID_SOCKET;
		iPhone->setClient(*ClientSocket);
		*/
		printf("Socket initialised in new memory: %d\n", ClientSocket);
		printf("Socket pointer initialised in new memory: %d\n", *ClientSocket);
	}
	delete ClientSocket;
	WSACleanup();

	printf("complete");
	HangCMD();

	delete iPhone;
	return 0;
}
