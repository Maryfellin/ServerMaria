#pragma comment( lib, "ws2_32.lib" )
#include <stdio.h>
#include <locale.h>
#include <winsock2.h>
#include <iostream>
#include <Ws2tcpip.h> 
#include <iomanip>
#include <cstring>

using namespace std;

enum Packet
{
	Message
};

char ServerIP[] = "127.0.0.1";
int Port = 1509;

SOCKET Connections[100]; /*коллекция сокетов, массив в котором подключенные пользователи*/
int ClientCount = 0; /*кол-во подключаемых пользователей, изначально 0*/

bool Send(int ID, int _int)
{
	int RetnCheck = send(Connections[ID], (char*)&_int, sizeof(int), NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Get(int ID, int & _int)
{
	int RetnCheck = recv(Connections[ID], (char*)&_int, sizeof(int), NULL); //приём
	if (RetnCheck == SOCKET_ERROR) //Если есть проблема с подключением
		return false;
	return true;
}

bool SendPacket(int ID, Packet _packettype)
{
	int RetnCheck = send(Connections[ID], (char*)&_packettype, sizeof(Packet), NULL); //отправить пакет
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool GetPacket(int ID, Packet & _packettype)
{
	int RetnCheck = recv(Connections[ID], (char*)&_packettype, sizeof(Packet), NULL); //получение пакета
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool SendString(int ID, std::string & _string)
{
	if (!SendPacket(ID, Message))
		return false;
	int bufferlength = _string.size();
	if (!Send(ID, bufferlength))
		return false;
	int RetnCheck = send(Connections[ID], _string.c_str(), bufferlength, NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}


bool GetString(int ID, std::string & _string)
{
	int bufferlength; //длина сообщения
	if (!Get(ID, bufferlength)) //получить длину буфера и сохранить его в переменной: bufferlength
		return false;
	char * buffer = new char[bufferlength + 1]; //выделение буфера
	buffer[bufferlength] = '\0'; //устанавливаем последний символ буфера нулевым
	int RetnCheck = recv(Connections[ID], buffer, bufferlength, NULL); //получаем сообщение и сохраняем в массиве буферов
	_string = buffer; //установить строку в полученное сообщение буфера
	delete[] buffer;
	if (RetnCheck == SOCKET_ERROR) //если соединение потеряно во время получения сообщения
		return false;
	return true;
}

bool ProcessPacket(int ID, Packet _packettype)
{
	switch (_packettype)
	{
	case Message:
	{
		std::string Message; //Строка для хранения полученного нами сообщения
		if (!GetString(ID, Message)) //Получить сообщение чата и сохранить его в переменной
			return false; 

		for (int i = 0; i < ClientCount; i++) // Далее нам нужно отправить сообщение каждому пользователю
		{
			if (i == ID) //Если соединение - это пользователь, который отправил сообщение ...
				continue;//Переход к следующему пользователю
			if (!SendString(i, Message))
			{
				std::cout << "Не удалось отправить сообщение клиента с ID: " << ID << " клиенту с ID: " << i << std::endl;
			}
		}
		std::cout << "Сообщение от клиента: " << ID << std::endl;
		break;
	}

	default:
	{
		std::cout << "Неизвестный пакет" << _packettype << std::endl;
		break;
	}
	}
	return true;
}

void ClientHandlerThread(int ID)
{
	Packet PacketType;
	while (true)
	{
		if (!GetPacket(ID, PacketType))
			break;
		if (!ProcessPacket(ID, PacketType))
			break;
	}
	std::cout << "Потеряна связь с клиентом: " << ID << std::endl;
	closesocket(Connections[ID]);
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

	cout << "Прослушивание входящих соединений...\n";

	SOCKET newConnection; //Сокет для подключения клиента
	int ConnectionCounter = 0;
	for (int i = 0; i < 100; i++)

	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Примите новое подключение
		if (newConnection == 0) //Если не удалось принять соединение клиента
		{
			std::cout << "Ошибка подключения клиента" << std::endl;
		}
		else //Если клиент принят
		{
			std::cout << "Клиент подключился! \n" << std::endl;
			Connections[i] = newConnection; //Установите сокет в массиве, чтобы быть самым новым соединением, прежде чем создавать поток для обработки сокета этого клиента.
			ClientCount += 1; //Увеличение общего количества подключенных клиентов
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL); //Создать поток для обработки этого клиента. Индекс в массиве сокетов для этого потока - это значение (i).
			std::string buftest = "";
			int size = buftest.size(); //Получить размер сообщения в байтах и сохранить его в int size
			std::string MOTD = "Добро пожаловать!";
			SendString(i, MOTD);
		}
	}
	system("pause");
	return 0;
}
