#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996) 

#define HOST "127.0.0.1"
#define PORT 7777

void initWinsock();
SOCKADDR_IN createServer();
SOCKET getCurrentConnection(SOCKADDR_IN server);

int main() {
	initWinsock();
	SOCKADDR_IN server = createServer();
	SOCKET connection = getCurrentConnection(server);
}

void initWinsock() {
	WSADATA wsaData;

	// Номер версии -  2.2
	WORD winsockVersion = MAKEWORD(2, 2);
	if (WSAStartup(winsockVersion, &wsaData) != 0) {
		puts("Winsock library load error");
		exit(1);
	}
}

SOCKADDR_IN createServer() {
	SOCKADDR_IN server;

	// Адрес, на котором будем слушать соединения
	server.sin_addr.s_addr = inet_addr(HOST);
	server.sin_port = PORT;
	// С какими протоколами будет работать
	server.sin_family = AF_INET;

	return server;
}

SOCKET getCurrentConnection(SOCKADDR_IN server) {
	int addresslen = sizeof(server);

	SOCKET incomingConnectionsListener = socket(AF_INET, SOCK_STREAM, NULL);
	bind(incomingConnectionsListener, (SOCKADDR*) &server, addresslen);

	// Слушаем максимально возможное число входящих соединений
	listen(incomingConnectionsListener, SOMAXCONN);

	SOCKET newConnection = accept(incomingConnectionsListener, (SOCKADDR*)&server, addresslen);
	if (!newConnection) {
		puts("Server listener error");
		exit(1);
	}

	return newConnection;
}