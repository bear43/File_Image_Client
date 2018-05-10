// File_Image_Client.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Client.cpp"
#include "Out.cpp"
#include "Request.h"

WSAData wsadata;
Client* client = NULL;
const char NetAddress::defaultIP[] = "127.0.0.1";
mutex Out::mtx;
bool isWorking = true;

int getCmd()
{
	printf("Menu:\n1. Send message cmd\n2. Send exit cmd\n3. Get CPU loading\n4. Get cores loading\nYour choice: ");
	int choice;
	scanf("%i", &choice);
	return choice;
}

int main(int argc, char* argv[])
{
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) == 0)
		Out::println("WSA inited!");
	else
		Out::println("Catch some error while WSA starting!\nError: %i", WSAGetLastError());
	client = new Client(new NetAddress("127.0.0.1", 8383));
	client->createSocket();
	client->bindSocket();
	client->setConnectAddress(new NetAddress("127.0.0.1", 8238));
	client->connect();
	int choice;
	Request req;
exitL:
	while (isWorking)
	{
		choice = getCmd();
		switch (choice)
		{
		case 0:
			isWorking = false;
			goto exitL;
		case 1:
			strcpy(req.header, HEADER_TO_MSG);
			break;
		case 2:
			strcpy(req.header, HEADER_TO_EXIT);
			break;
		case 3:
			strcpy(req.header, HEADER_TO_GET_CPU_LOADING);
			break;
		case 4:
			strcpy(req.header, HEADER_TO_GET_CORE_LOADING);
			break;
		default:
			printf("Wrong menu item! Repeat...\n");
			break;
		}
		client->sendRequest(req);
	}
	WSACleanup();
    return 0;
}

