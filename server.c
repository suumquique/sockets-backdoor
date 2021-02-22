#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <stdbool.h>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996) 

#define HOST "192.168.43.153"
#define PORT 7777
#define MAX_PATH_LENGTH 200
#define FILE_REMOVING_ERROR 1

void initWinsock();
SOCKADDR_IN createServer();
SOCKET getCurrentConnection(SOCKADDR_IN server);
int deleteFileFromClient(SOCKET currentConnection);

int main() {
	initWinsock();
	SOCKADDR_IN server = createServer();
	SOCKET connection = getCurrentConnection(server);
	
	int again = 1;
	while (again) {
		int status = deleteFileFromClient(connection);
		switch (status) {
		case 0:
			puts("\nFile deletion successful");
			break;
		case SOCKET_ERROR:
			puts("\nClient connection interrupted");
			break;
		case FILE_REMOVING_ERROR:
			puts("\nUnable to delete the specified file");
			break;
		default:
			puts("\nUnknown error");
		}

		puts("\nContinue deleting files? 0 - No, 1 - Yes");
		scanf("%d", &again);
		getchar();
	}
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

	puts("Waiting for client connection...");

	SOCKET newConnection = accept(incomingConnectionsListener, (SOCKADDR*)&server, &addresslen);
	if (!newConnection) {
		puts("Server listener error");
		exit(1);
	}
	else {
		puts("Connect to client established\n");
	}

	return newConnection;
}

int deleteFileFromClient(SOCKET currentConnection) {
	char filenameToDelete[MAX_PATH_LENGTH], result[2];
	int resultStatus;
	puts("Enter the path to the file to be deleted from client computer:");
	gets_s(filenameToDelete, MAX_PATH_LENGTH);

	// Отправляем клиенту название файла, который требуется удалить
	if (SOCKET_ERROR == send(currentConnection, filenameToDelete, MAX_PATH_LENGTH, NULL)) {
		puts("Error: can`t send message to client");
		return SOCKET_ERROR;
	}

	// Получаем от клиента информацию о статусе выполнения (удачно удалено или нет). 0 - удачно, остальные статусы - нет.
	if (SOCKET_ERROR == recv(currentConnection, result, sizeof(result), NULL)) {
		puts("Error getting status from client");
		return SOCKET_ERROR;
	}

	/* Статус передается в строке (массиве типа char из двух элементов), поскольку функции для передачи (send и recv) работают со строками.
	* После передачи статус переводится назад в числовое значение */
	resultStatus = result[0] - '0';
	printf("Result status: %d\n", resultStatus);
	return !resultStatus ? resultStatus : FILE_REMOVING_ERROR;
}