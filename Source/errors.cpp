#include "sideload.h"
#include "errors.h"
#include "globals.h"


void exceptionHandler(int signum) {
	printf("Signal: %d\n", signum);
	g_exit = true;
	HangCMD();
}

void HangCMD() {
	char *chInput = new char[5];
	scanf("%s", chInput);
	delete[] chInput;
}


int callWinsockError(int errorCode, const char *errorType, bool getError = false, int WSAAPI cleanupFunc() = nullptr, struct addrinfo *addrToFree = nullptr, SOCKET *socketToClose = nullptr) {
	if (getError == false) printf(errorType, ": %d\n", errorCode); else printf(errorType, ": %d\n", WSAGetLastError());
	if (addrToFree) freeaddrinfo(addrToFree);
	if (socketToClose) closesocket(*socketToClose);
	if (cleanupFunc) (*cleanupFunc)();
	HangCMD();
	return 1;
}


int checkWinsockError(int errorCode, const char *errorType, bool getError, int WSAAPI cleanupFunc(), struct addrinfo *addrToFree, SOCKET *socketToClose, bool sockError) {
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