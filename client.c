#include <string.h>
#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996) 

#define HOST "127.0.0.1"
#define PORT 7777
#define MAX_PATH_LENGTH 200

void initWinsock();
SOCKADDR_IN createServer();

int main() {
	initWinsock();
	SOCKADDR_IN server = createServer();
	
	SOCKET connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(connection, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR) {
		puts("Connect failed");
		exit(WSAGetLastError());
	}
	else {
		puts("Connected");
	}

	char filenameToDelete[MAX_PATH_LENGTH];
	// Получаем с сервера сообщение с путем к файлу, который требуется удалить
	recv(connection, filenameToDelete, sizeof(filenameToDelete), NULL);

	// Проверяем, успешно ли удалился файл
	int resultStatus = remove(filenameToDelete);
	printf("Filename: %s\nResult: %d", filenameToDelete, resultStatus);
	/* Статус передается в строке (массиве типа char из двух элементов), поскольку функции для передачи (send и recv) работают со строками.
	* Для передачи на сервер статус удаления файла переводится в строку из числа*/
	char result[2] = { resultStatus + '0', '\0' };

	// Отправляем информацию о том, успешно ли выполнена операция удаления файла, на сервер
	send(connection, result, sizeof(result), NULL);
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