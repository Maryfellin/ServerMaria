
#pragma comment( lib, "ws2_32.lib" )
#include <stdio.h>
#include <locale.h>
#include <winsock2.h>
#include <Ws2tcpip.h> /*для сервера*/

using namespace std;

int main()
{
	setlocale(LC_ALL, "russian");
	WSADATA WSAData;

	SOCKET server, client; //сокеты

	/*объект, который содержит информацию об адресе.*/
	SOCKADDR_IN serverAddr, clientAddr;

	WSAStartup(MAKEWORD(2, 0), &WSAData); //указание версии winsock
	server = socket(AF_INET, SOCK_STREAM, 0);

	/*Сервер будет запущен на порту 5555 и будет отображать сообщения клиента*/
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);

	/*Связываем сокет с IP - адресом и номером порта*/
	bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	listen(server, 0);

	printf("Прослушивание входящих соединений... \n");

	char buffer[1024];
	int clientAddrSize = sizeof(clientAddr);
	/*Функция accept осуществляет связь с сокетом*/
	if ((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
	{
		printf("Клиент подключился! \n");
		recv(client, buffer, sizeof(buffer), 0);
		printf("Сообщение: ");
		printf(buffer);
		printf("\n");
		memset(buffer, 0, sizeof(buffer));

		closesocket(client);
		printf("Клиент не подключен");
		getchar();
	}
}