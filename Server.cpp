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

SOCKET Connections[1000]; /*��������� �������, ������ � ������� ������������ ������������*/
int ClientCount = 0; /*���-�� ������������ �������������, ���������� 0*/

//SOCKET server, client;
//SOCKET Listen; /*����� ��� �����������*/

bool ProcessPacket(int ID, Packet packettype)
{
	switch (packettype)
	{
	case P_ChatMessage:
	{
		int bufferlength; //����� ������
		//��� ��������� ������
		int recvcheck = recv(Connections[ID], (char*)&bufferlength, sizeof(int), NULL); //�������� ����� ������
		char * buffer = new char[bufferlength]; //�������� �����
		recv(Connections[ID], buffer, bufferlength, NULL); //�������� ��������� ������ �� �������
		for (int i = 0; i < ClientCount; i++) //��� ������� ����������� ����������
		{
			if (i == ID) //�� ����������� ��������� ���� ���� �� ������������, ������� ��� ��������
				continue; //���������� ������������
			Packet chatmessagepacket = P_ChatMessage; //������� ����� ��������� ����, ������� ����� ���������
			send(Connections[i], (char*)&chatmessagepacket, sizeof(Packet), NULL); //��������� ����� ��������� ����
			send(Connections[i], (char*)&bufferlength, sizeof(int), NULL);//��������� ����� ������ ������� � �������� i
			send(Connections[i], buffer, bufferlength, NULL);//��������� ��������� ������� �� ������� i
			
		}
		delete[] buffer; // ���������� �����, ����� ���������� ������ ������
		break;
	}
	default:
		printf ("����������� �����.");
		break;
	}
	return true;
}

void ClientHandlerThread(int ID)
{
	while (true)
	{
		//������� �������� ��� ������
		Packet packettype;
		recv(Connections[ID], (char*)&packettype, sizeof(Packet), NULL); //��������� ���� ������ �� �������

		//��� ������ � ��� ����� ��� ������, ���������� �����
		if (!ProcessPacket(ID, packettype)) 
			break; 
	}
	closesocket(Connections[ID]); //������� �����, ������� ������������� ��� ����������� �������
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

	printf("������������� �������� ����������...");

	SOCKET newConnection; //����� ��� ����������� �������
	int ConnectionCounter = 0; 
	for (int i = 0; i < 100; i++)

	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //������� ����� �����������
		if (newConnection == 0) //���� �� ������� ������� ���������� �������
		{
			printf ("������ ����������� �������");
		}
		else //���� ������ ������
		{
			printf ("������ �����������!");
			Connections[i] = newConnection; //���������� ����� � �������, ����� ���� ����� ����� �����������, ������ ��� ��������� ����� ��� ��������� ������ ����� �������.
			ClientCount += 1; //���������� ������ ���������� ������������ ��������
			cout << "����� ��������: " << ClientCount;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL); //������� ����� ��� ��������� ����� �������. ������ � ������� ������� ��� ����� ������ - ��� �������� (i).
			std:: string buftest = "Welcome!";
			int size = buftest.size(); //�������� ������ ��������� � ������ � ��������� ��� � int size
			Packet chatmessagepacket = P_ChatMessage; //������� ��� ������: Chat Message, ������������ �� ������
			send(Connections[i], (char*)&chatmessagepacket, sizeof(Packet), NULL); //��������� ��� ������: Chat Message
			send(Connections[i], (char*)&size, sizeof(int), NULL); //��������� ������ ���������
			send(Connections[i], buftest.c_str(), buftest.size(), NULL); //��������� ���������

			Packet testpacket = P_Test;
			send(Connections[i], (char*)&testpacket, sizeof(Packet), NULL); //��������� �������� �����
		}
	}
	system("pause");
	return 0;

}
