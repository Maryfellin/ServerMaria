
#pragma comment( lib, "ws2_32.lib" )
#include <stdio.h>
#include <locale.h>
#include <winsock2.h>
#include <Ws2tcpip.h> /*��� �������*/

using namespace std;

int main()
{
	setlocale(LC_ALL, "russian");
	WSADATA WSAData;

	SOCKET server, client; //������

	/*������, ������� �������� ���������� �� ������.*/
	SOCKADDR_IN serverAddr, clientAddr;

	WSAStartup(MAKEWORD(2, 0), &WSAData); //�������� ������ winsock
	server = socket(AF_INET, SOCK_STREAM, 0);

	/*������ ����� ������� �� ����� 5555 � ����� ���������� ��������� �������*/
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);

	/*��������� ����� � IP - ������� � ������� �����*/
	bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	listen(server, 0);

	printf("������������� �������� ����������... \n");

	char buffer[1024];
	int clientAddrSize = sizeof(clientAddr);
	/*������� accept ������������ ����� � �������*/
	if ((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
	{
		printf("������ �����������! \n");
		recv(client, buffer, sizeof(buffer), 0);
		printf("���������: ");
		printf(buffer);
		printf("\n");
		memset(buffer, 0, sizeof(buffer));

		closesocket(client);
		printf("������ �� ���������");
		getchar();
	}
}