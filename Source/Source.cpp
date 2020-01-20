#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "source.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

void HangCMD() {
	char *chInput = new char[5];
	scanf("%s", chInput);
	delete[] chInput;
}


int main() {
	WSADATA wsaData;

	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup error: %d\n", iResult);
		HangCMD();
		return 1;
	}

	const char cchDEF_PORT[] = "8888";

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, cchDEF_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo error: %d\n", iResult);
		WSACleanup();
		HangCMD();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Socket Error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		HangCMD();
		return 1;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("Bind Error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		HangCMD();
		return 1;
	}

	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen Error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		HangCMD();
		return 1;
	}

	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;

	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("Accept Error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		HangCMD();
		return 1;
	}

	const int DEF_BUFLEN = 1024;
	char recvbuf[DEF_BUFLEN];
	char sendbuf[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: Closed\r\nContent-Length: 75\r\n\r\n<html>\r\n<head>\r\n</head>\r\n<body>\r\n<h1>Carpe Diem! :(</h1>\r\n</body>\r\n</html>";
	int sendbuflen = sizeof(sendbuf) / sizeof(char);
	int iSendResult;
	int recvbuflen = DEF_BUFLEN;

	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes recieved: %d\n", iResult);
			printf("%s\n", recvbuf);

			iSendResult = send(ClientSocket, sendbuf, sendbuflen, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("Send Error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				HangCMD();
				return 1;
			}

			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0) {
			printf("Connection closing...\n");
		}
		else {
			printf("Recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			HangCMD();
			return 1;
		}
	} while (iResult > 0);

	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("Shutdown Error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		HangCMD();
		return 1;
	}

	closesocket(ClientSocket);
	WSACleanup();

	printf("complete");
	HangCMD();

	return 0;
}
