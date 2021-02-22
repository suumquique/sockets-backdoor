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
	// �������� � ������� ��������� � ����� � �����, ������� ��������� �������
	recv(connection, filenameToDelete, sizeof(filenameToDelete), NULL);

	// ���������, ������� �� �������� ����
	int resultStatus = remove(filenameToDelete);
	printf("Filename: %s\nResult: %d", filenameToDelete, resultStatus);
	char result[2] = { resultStatus + '0', '\0' };

	// ���������� ���������� � ���, ������� �� ��������� �������� �������� �����, �� ������
	send(connection, result, sizeof(result), NULL);
}

void initWinsock() {
	WSADATA wsaData;

	// ����� ������ -  2.2
	WORD winsockVersion = MAKEWORD(2, 2);
	if (WSAStartup(winsockVersion, &wsaData) != 0) {
		puts("Winsock library load error");
		exit(1);
	}
}

SOCKADDR_IN createServer() {
	SOCKADDR_IN server;

	// �����, �� ������� ����� ������� ����������
	server.sin_addr.s_addr = inet_addr(HOST);
	server.sin_port = PORT;
	// � ������ ����������� ����� ��������
	server.sin_family = AF_INET;

	return server;
}