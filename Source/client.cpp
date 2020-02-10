#include "sideload.h"
#include "client.h"


Client::Client(SOCKET *ClientArg, SOCKET *ListenArg) {
	this->ClientSocket = ClientArg;
	this->ListenSocket = ListenArg;
	printf("Socket within Client Constructer: %d\n", ClientSocket);
	printf("Socket supplied to Client Constructer: %d\n", ClientArg);
}


Client::~Client() {
}

void Client::carrRet() {
	strcat(sendBuf, "\r\n");
	return;
}

int Client::acceptConn() {
	*this->ClientSocket = accept(*this->ListenSocket, NULL, NULL);
	printf("Socket within accept method after accept: %d\n", ClientSocket);
	if (checkWinsockError(*this->ClientSocket, "accept error", true, WSACleanup, nullptr, this->ListenSocket, true)) return 1;
	return 0;
}
int Client::recvData(SOCKET *ClientArg) {
	memset(recvBuf, 0, sizeof(recvBuf));
	int tmp = recv(*ClientSocket, recvBuf, g_DEF_BUFLEN, 0);
	return tmp;
}


int Client::checkMsg(int length) {
	if (length > 0) {
		printf("\nBytes recieved: %d\n", length);
		printf("\n%s\n", recvBuf);
		return 0;
	}
	else if (length == 0) {
		printf("Length: %d\n", length);
		printf("Connection closing...\n");
		return 1;
	}
	else {
		printf("Bytes recieved causing error: %d\n", length);
		callWinsockError(1, "recv failed", true, WSACleanup, nullptr, ClientSocket);
		return 1;
	}
	return 0;
}

int Client::sendData(char *statusCode, char *contentType, char *connStatus, char *msgBody, char *msgLen) {
	memset(sendBuf, 0, sizeof(sendBuf));
	// alternate string formatting method
	// printf("HTTPS/1.1 %s\r\nContent-Type: %s\r\nConnection: %s\r\nContent-Length: %s\r\n\r\n%s\n\n", statusCode, contentType, connStatus, msgLen, msgBody);

	printf("\nStatus Code:       %s\n", statusCode);
	printf("Content Type:      %s\n", contentType);
	printf("Connection Status: %s\n", connStatus);
	// printf("Message Body:      %s\n", msgBody);
	printf("Message Length:    %s\n\n", msgLen);

	// strcpy(sendBuf, ("HTTPS/1.1 %s\r\nContent-Type: %s\r\nConnection: %s\r\nContent-Length: %s\r\n\r\n%s", statusCode, contentType, msgBody, msgLen, connStatus));

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

	printf("\nMsg to send:\n\n%s\n\n", sendBuf);

	iSendResult = send(*ClientSocket, sendBuf, g_DEF_BUFLEN, 0);
	if (checkWinsockError(iSendResult, "send error", true, WSACleanup, nullptr, ClientSocket, true) == 1) return 1;

	printf("Bytes sent: %d\n", iSendResult);
	return 0;
}


int Client::setClient(SOCKET ClientArg) {
	this->ClientSocket = &ClientArg;
	return 0;
}

int Client::setListen(SOCKET* ListenArg) {
	this->ListenSocket = ListenArg;
	return 0;
}