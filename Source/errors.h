#pragma once

void exceptionHandler(int);

void HangCMD();

int callWinsockError(int errorCode, const char *errorType, bool getError, int WSAAPI cleanupFunc(), struct addrinfo *addrToFree, SOCKET *socketToClose);

int checkWinsockError(int errorCode, const char *errorType, bool getError = false, int WSAAPI cleanupFunc() = nullptr, struct addrinfo *addrToFree = nullptr, SOCKET *socketToClose = nullptr, bool sockError = false);