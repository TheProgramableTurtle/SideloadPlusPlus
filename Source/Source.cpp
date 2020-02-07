#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "source.h"

char g_recvbuf[g_DEF_BUFLEN];

int checkWinsockError(int errorCode, const char *errorType, bool getError = false, int WSAAPI cleanupFunc() = nullptr, struct addrinfo *addrToFree = nullptr, SOCKET *socketToClose = nullptr, bool sockError = false) {
	if ((sockError == true && (errorCode == INVALID_SOCKET || errorCode == SOCKET_ERROR)) || (sockError == false && errorCode != INVALID_SOCKET && errorCode != SOCKET_ERROR && errorCode != 0)) {
		if (getError == false) printf(errorType, ": %d\n", errorCode); else printf(errorType, ": %d\n", WSAGetLastError());
		if (addrToFree) freeaddrinfo(addrToFree);
		if (socketToClose) closesocket(*socketToClose);
		if (cleanupFunc) (*cleanupFunc)();
		HangCMD();
		return 1;
	}
	return 0;
}


Client::Client(SOCKET *ClientArg, SOCKET *ListenArg) {
	this->ClientSocket = ClientArg;
	this->ListenSocket = ListenArg;
	printf("%d\n", ClientSocket);
	printf("%d\n", ClientArg);

	strcpy(recvBuf, "Turtle Boi");
}


Client::~Client() {
}

void Client::carrRet() {
	strcat(sendBuf, "\r\n");
	return;
}

int Client::acceptConn() {
	*ClientSocket = accept(*ListenSocket, NULL, NULL);
	if (checkWinsockError(*ClientSocket, "accept error", true, WSACleanup, nullptr, ListenSocket, true)) return 1;
	return 0;
}
int Client::recvData(SOCKET *ClientArg) {
	memset(recvBuf, 0, sizeof(recvBuf));
	int tmp = recv(*ClientSocket, recvBuf, g_DEF_BUFLEN, 0);
	return tmp;
}


int Client::checkMsg(int length) {
	printf("%d\n", length);
	if (length > 0) {
		printf("Bytes recieved: %d\n", length);
		printf("%s\n", recvBuf);
		return 0;
	}
	else if (length == 0) {
		printf("Connection closing...\n");
		return 1;
	}
	else {
		callWinsockError(1, "recv failed", true, WSACleanup, nullptr, ClientSocket);
		return 1;
	}
	return 0;
}

int Client::sendData(char *statusCode, char *contentType, char *connStatus, char *msgBody, char *msgLen) {
	memset(sendBuf, 0, sizeof(sendBuf));
	printf("HTTPS/1.1 %s\r\nContent-Type: %s\r\nConnection: %s\r\nContent-Length: %s\r\n\r\n%s\n", statusCode, contentType, msgBody, msgLen, connStatus);

	printf("HTTPS/1.1 %s\r\nContent-Type: %s\r\nConnection: %s\r\nContent-Length: %s\r\n\r\n%s\n", "hi", "hello", "wassup", "yolo", "ejnenf");

	printf("%s\n", statusCode);
	printf("%s\n", contentType);
	printf("%s\n", connStatus);
	printf("%s\n", msgBody);
	printf("%s\n", msgLen);

	// strcpy(sendBuf, ("HTTPS/1.1 %s\r\nContent-Type: %s\r\nConnection: %s\r\nContent-Length: %s\r\n\r\n%s", statusCode, contentType, msgBody, msgLen, connStatus));
	printf("%d\n", *&sendBuf);
	
	strcat(sendBuf, "HTTP/1.1 ");
	strcat(sendBuf, statusCode);
	this->carrRet();
	strcat(sendBuf, "Content-Type: ");
	strcat(sendBuf, contentType);
	this->carrRet();
	strcat(sendBuf, "Connection: ");
	strcat(sendBuf, connStatus);
	this->carrRet();
	strcat(sendBuf, "Content-Length: ");
	strcat(sendBuf, msgLen);
	this->carrRet();
	this->carrRet();
	strcat(sendBuf, msgBody);
	
	printf("%d\n", *&sendBuf);
	printf("Msg to send:\n%s", sendBuf);

	iSendResult = send(*ClientSocket, sendBuf, g_DEF_BUFLEN, 0);
	if (checkWinsockError(iSendResult, "send error", true, WSACleanup, nullptr, ClientSocket, true) == 1) return 1;

	printf("Bytes sent: %d\n", iSendResult);
	return 0;
}


void HangCMD() {
	char *chInput = new char[5];
	scanf("%s", chInput);
	delete[] chInput;
}


void callNull() {
}


int callWinsockError(int errorCode, const char *errorType, bool getError=false, int WSAAPI cleanupFunc()=nullptr, struct addrinfo *addrToFree=nullptr, SOCKET *socketToClose=nullptr) {
	if (getError == false) printf(errorType, ": %d\n", errorCode); else printf(errorType, ": %d\n", WSAGetLastError());
	if (addrToFree) freeaddrinfo(addrToFree);
	if (socketToClose) closesocket(*socketToClose);
	if (cleanupFunc) (*cleanupFunc)();
	HangCMD();
	return 1;
}


struct ResponseValues {
	char statusCode[20];
	char contentType[30];
	char msgBody[100];
	char msgLen[5];
	char connStatus[20];
};


int main() {
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

	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;
	printf("%d\n", &ClientSocket);
	Client iPhone(&ClientSocket, &ListenSocket);
	if (iPhone.acceptConn()) return 1;

	iResult = iPhone.recvData(&ClientSocket);
	printf("error: %d\n", WSAGetLastError());
	
	ResponseValues values;
	strcpy(values.statusCode, "200 OK");
	strcpy(values.contentType, "text/html");
	strcpy(values.connStatus, "Closed");
	strcpy(values.msgBody, "<html>\r\n<head>\r\n</head>\r\n<body>\r\n<h1>Carpe Diem!:(</h1>\r\n</body>\r\n</html>");
	strcpy(values.msgLen, "82");

	while (iPhone.checkMsg(iResult) == 0) {
		iPhone.sendData(values.statusCode, values.contentType, values.connStatus, values.msgBody, values.msgLen);
		iResult = iPhone.recvData(&ClientSocket);
	}
	printf("error: %d\n", WSAGetLastError());

	iResult = shutdown(ClientSocket, SD_SEND);
	if (checkWinsockError(iResult, "Shutdown Error", true, WSACleanup, nullptr, &ClientSocket)) return 1;

	closesocket(ClientSocket);
	WSACleanup();

	printf("complete");
	HangCMD();

	return 0;
}
