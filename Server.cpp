#pragma comment( lib, "ws2_32.lib" )
#include <stdio.h>
#include <locale.h>
#include <winsock2.h>
#include <Ws2tcpip.h> 

using namespace std;

char ServerIP[] = "127.0.0.1";
int Port = 1509;

int main()
{
	setlocale(LC_ALL, "russian");
	WSADATA WSAData;

	SOCKET server, client;

	SOCKADDR_IN serverAddr, clientAddr;

	WSAStartup(MAKEWORD(2, 0), &WSAData);
	server = socket(AF_INET, SOCK_STREAM, 0);

	/*Указываем IP-адрес сокета, к которому он должен подключиться и порт*/
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(ServerIP);
	serverAddr.sin_port = htons(Port);

	/*Связываем сокет с IP-адресом и номером порта*/
	bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	listen(server, 0);

	/*связь с сокетом*/
	printf("Прослушивание входящих соединений... \n");

	/*создание буфера*/
	char buffer[1024];
	int clientAddrSize = sizeof(clientAddr);
	if ((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
	{
		printf("Клиент подключился! \n");
		recv(client, buffer, sizeof(buffer), 0);
		printf("Сообщение отправлено: ");
		printf(buffer);

		printf("\n");
		memset(buffer, 0, sizeof(buffer));
		getchar();
		closesocket(client);
		printf("Клиент не подключен");
	}
	return 1;

}
