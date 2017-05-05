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

SOCKET Connections[100]; /*��������� �������, ������ � ������� ������������ ������������*/
int ClientCount = 0; /*���-�� ������������ �������������, ���������� 0*/

bool Send(int ID, int _int)
{
	int RetnCheck = send(Connections[ID], (char*)&_int, sizeof(int), NULL);
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool Get(int ID, int & _int)
{
	int RetnCheck = recv(Connections[ID], (char*)&_int, sizeof(int), NULL); //����
	if (RetnCheck == SOCKET_ERROR) //���� ���� �������� � ������������
		return false;
	return true;
}

bool SendPacket(int ID, Packet _packettype)
{
	int RetnCheck = send(Connections[ID], (char*)&_packettype, sizeof(Packet), NULL); //��������� �����
	if (RetnCheck == SOCKET_ERROR)
		return false;
	return true;
}

bool GetPacket(int ID, Packet & _packettype)
{
	int RetnCheck = recv(Connections[ID], (char*)&_packettype, sizeof(Packet), NULL); //��������� ������
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
	int bufferlength; //����� ���������
	if (!Get(ID, bufferlength)) //�������� ����� ������ � ��������� ��� � ����������: bufferlength
		return false;
	char * buffer = new char[bufferlength + 1]; //��������� ������
	buffer[bufferlength] = '\0'; //������������� ��������� ������ ������ �������
	int RetnCheck = recv(Connections[ID], buffer, bufferlength, NULL); //�������� ��������� � ��������� � ������� �������
	_string = buffer; //���������� ������ � ���������� ��������� ������
	delete[] buffer;
	if (RetnCheck == SOCKET_ERROR) //���� ���������� �������� �� ����� ��������� ���������
		return false;
	return true;
}

bool ProcessPacket(int ID, Packet _packettype)
{
	switch (_packettype)
	{
	case Message:
	{
		std::string Message; //������ ��� �������� ����������� ���� ���������
		if (!GetString(ID, Message)) //�������� ��������� ���� � ��������� ��� � ����������
			return false; 

		for (int i = 0; i < ClientCount; i++) // ����� ��� ����� ��������� ��������� ������� ������������
		{
			if (i == ID) //���� ���������� - ��� ������������, ������� �������� ��������� ...
				continue;//������� � ���������� ������������
			if (!SendString(i, Message))
			{
				std::cout << "�� ������� ��������� ��������� ������� � ID: " << ID << " ������� � ID: " << i << std::endl;
			}
		}
		std::cout << "��������� �� �������: " << ID << std::endl;
		break;
	}

	default:
	{
		std::cout << "����������� �����" << _packettype << std::endl;
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
	std::cout << "�������� ����� � ��������: " << ID << std::endl;
	closesocket(Connections[ID]);
}



int main()
{
	setlocale(LC_ALL, "russian");
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKADDR_IN addr;
	int addrlen = sizeof(addr); //����� ������ (��������� ��� �������� ������)
	addr.sin_addr.s_addr = inet_addr(ServerIP);
	addr.sin_port = htons(Port); //Port
	addr.sin_family = AF_INET; //IPv4 �����

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); // ������� ����� ��� ������������� ����� �����������
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); // ���������� ������ � �������
	listen(sListen, SOMAXCONN);

	cout << "������������� �������� ����������...\n";

	SOCKET newConnection; //����� ��� ����������� �������
	int ConnectionCounter = 0;
	for (int i = 0; i < 100; i++)

	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //������� ����� �����������
		if (newConnection == 0) //���� �� ������� ������� ���������� �������
		{
			std::cout << "������ ����������� �������" << std::endl;
		}
		else //���� ������ ������
		{
			std::cout << "������ �����������! \n" << std::endl;
			Connections[i] = newConnection; //���������� ����� � �������, ����� ���� ����� ����� �����������, ������ ��� ��������� ����� ��� ��������� ������ ����� �������.
			ClientCount += 1; //���������� ������ ���������� ������������ ��������
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL); //������� ����� ��� ��������� ����� �������. ������ � ������� ������� ��� ����� ������ - ��� �������� (i).
			std::string buftest = "";
			int size = buftest.size(); //�������� ������ ��������� � ������ � ��������� ��� � int size
			std::string MOTD = "����� ����������!";
			SendString(i, MOTD);
		}
	}
	system("pause");
	return 0;
}
