#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")

#define HOST "127.0.0.1"
#define PORT 7777

void initWinsock();
SOCKADDR_IN createServer();

int main() {
	initWinsock();
	SOCKADDR_IN server = createServer();
}

void initWinsock() {
	WSADATA wsaData;
	WORD winsockVersion = MAKEWORD(2, 2);
	if (WSAStartup(winsockVersion, &wsaData) != 0) {
		puts("Winsock library load error");
		exit(1);
	}
}

SOCKADDR_IN createServer() {
	SOCKADDR_IN server;
	server.sin_addr.s_addr = inet_addr(HOST);
	server.sin_port = PORT;
	server.sin_family = AF_INET;

	return server;
}
