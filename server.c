#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")

void initWinsock();

int main() {
	initWinsock();
}

void initWinsock() {
	WSADATA wsaData;
	WORD winsockVersion = MAKEWORD(2, 2);
	if (WSAStartup(winsockVersion, &wsaData) != 0) {
		puts("Winsock library load error");
		exit(1);
	}
}