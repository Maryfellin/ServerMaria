#pragma comment( lib, "ws2_32.lib" )
#include <stdio.h>
#include <locale.h>
#include <winsock2.h>
#include <iostream>
#include <Ws2tcpip.h> 

using namespace std;

enum Packet
{
	P_ChatMessage,
	P_Test
};

char ServerIP[] = "127.0.0.1";
int Port = 1509;

SOCKET Connections[1000]; /*коллекция сокетов, массив в котором подключенные пользователи*/
int ClientCount = 0; /*кол-во подключаемых пользователей, изначально 0*/

//SOCKET server, client;
//SOCKET Listen; /*сокет для подключения*/

bool ProcessPacket(int ID, Packet packettype)
{
	switch (packettype)
	{
	case P_ChatMessage:
	{
		int bufferlength; //Длина строки
		//для получения строки
		int recvcheck = recv(Connections[ID], (char*)&bufferlength, sizeof(int), NULL); //Получить длину буфера
		char * buffer = new char[bufferlength]; //Выделить буфер
		recv(Connections[ID], buffer, bufferlength, NULL); //Получить сообщение буфера от клиента
		for (int i = 0; i < ClientCount; i++) //Для каждого клиентского соединения
		{
			if (i == ID) //Не отправляйте сообщение чату тому же пользователю, который его отправил
				continue; //Пропустить пользователя
			Packet chatmessagepacket = P_ChatMessage; //Создать пакет сообщений чата, который будет отправлен
			send(Connections[i], (char*)&chatmessagepacket, sizeof(Packet), NULL); //Отправить пакет сообщений чата
			send(Connections[i], (char*)&bufferlength, sizeof(int), NULL);//Отправить длину буфера клиенту с индексом i
			send(Connections[i], buffer, bufferlength, NULL);//Отправить сообщение клиенту по индексу i
			
		}
		delete[] buffer; // Освободить буфер, чтобы остановить утечку памяти
		break;
	}
	default:
		printf ("Неизвестный пакет.");
		break;
	}
	return true;
}

void ClientHandlerThread(int ID)
{
	while (true)
	{
		//Сначала получаем тип пакета
		Packet packettype;
		recv(Connections[ID], (char*)&packettype, sizeof(Packet), NULL); //Получение типа пакета от клиента

		//Как только у нас будет тип пакета, обработаем пакет
		if (!ProcessPacket(ID, packettype)) 
			break; 
	}
	closesocket(Connections[ID]); //Закрыть сокет, который использовался для подключения клиента
}

int main()
{
	setlocale(LC_ALL, "russian");
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKADDR_IN addr;
	int addrlen = sizeof(addr); //Длина адреса (требуется для принятия вызова)
	addr.sin_addr.s_addr = inet_addr(ServerIP); 
	addr.sin_port = htons(Port); //Port
	addr.sin_family = AF_INET; //IPv4 сокет

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); // Создаем сокет для прослушивания новых подключений
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); // Связывание адреса с сокетом
	listen(sListen, SOMAXCONN); 

	printf("Прослушивание входящих соединений...");

	SOCKET newConnection; //Сокет для подключения клиента
	int ConnectionCounter = 0; 
	for (int i = 0; i < 100; i++)

	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Примите новое подключение
		if (newConnection == 0) //Если не удалось принять соединение клиента
		{
			printf ("Ошибка подключения клиента");
		}
		else //Если клиент принят
		{
			printf ("Клиент подключился!");
			Connections[i] = newConnection; //Установите сокет в массиве, чтобы быть самым новым соединением, прежде чем создавать поток для обработки сокета этого клиента.
			ClientCount += 1; //Увеличение общего количества подключенных клиентов
			cout << "Всего клиентов: " << ClientCount;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL); //Создать поток для обработки этого клиента. Индекс в массиве сокетов для этого потока - это значение (i).
			std:: string buftest = "Welcome!";
			int size = buftest.size(); //Получить размер сообщения в байтах и сохранить его в int size
			Packet chatmessagepacket = P_ChatMessage; //Создать тип пакета: Chat Message, отправляемое на сервер
			send(Connections[i], (char*)&chatmessagepacket, sizeof(Packet), NULL); //Отправить тип пакета: Chat Message
			send(Connections[i], (char*)&size, sizeof(int), NULL); //отправить размер сообщения
			send(Connections[i], buftest.c_str(), buftest.size(), NULL); //отправить сообщение

			Packet testpacket = P_Test;
			send(Connections[i], (char*)&testpacket, sizeof(Packet), NULL); //отправить тестовый пакет
		}
	}
	system("pause");
	return 0;

}
