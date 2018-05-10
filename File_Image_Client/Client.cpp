#include "stdafx.h"
#include "Out.cpp"
#include "Request.h"
#include <future>
#include <thread>
#include <chrono>

using namespace std;

struct NetAddress
{
private:
	static const char defaultIP[];
	static const int defaultPort = 7377;
	sockaddr_in socketData;
	char* ipAddress;
	int port;
public:
	/*Заполняет данные сокета из приватных полей класса*/
	void fillSocketData()
	{
		fillSocketData(ipAddress, port, false);
	}
	/*Заполняет данные сокета из аргументов*/
	void fillSocketData(char* ipAddress, int port, bool refreshClassData)
	{
		socketData.sin_addr.S_un.S_addr = inet_addr(ipAddress);
		socketData.sin_port = htons(port);
		socketData.sin_family = AF_INET;
		if (refreshClassData)
		{
			this->ipAddress = ipAddress;
			this->port = port;
		}
	}
	void fillFieldsFromData(sockaddr_in socketData)
	{
		ipAddress = inet_ntoa(socketData.sin_addr);
		port = ntohs(socketData.sin_port);
	}
	void fillFieldsFromSocketData()
	{
		fillFieldsFromData(socketData);
	}
	sockaddr_in& getSocketData()
	{
		return socketData;
	}
	char* getIPAddress()
	{
		return ipAddress;
	}
	void setIPAddress(char* ipAddress)
	{
		this->ipAddress = ipAddress;
	}
	int getPort()
	{
		return port;
	}
	void setPort(int port)
	{
		this->port = port;
	}
	NetAddress(char* ipAddress, int port)
	{
		this->ipAddress = ipAddress;
		this->port = port;
		fillSocketData();
	}
	NetAddress(sockaddr_in address)
	{
		socketData = address;
		fillFieldsFromSocketData();
	}
	NetAddress() : NetAddress((char*)defaultIP, defaultPort)
	{

	}
};

class Client
{
private:
	mutex mtx;
	NetAddress* myAddress;
	NetAddress* connectAddress;
	SOCKET socket;
	SOCKET listenSocket;
	bool isWorking = true;
public:
	Client(NetAddress* address)
	{
		this->myAddress = address;
	}
	Client(sockaddr_in address) : Client(new NetAddress(address))
	{
	}
	Client() : Client(new NetAddress())
	{
	}
	~Client()
	{
		isWorking = false;
		closesocket(socket);
		delete myAddress;
	}
	bool createSocket()
	{
		socket = ::socket(AF_INET, SOCK_STREAM, 0);
		listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
		if (socket == INVALID_SOCKET || listenSocket == INVALID_SOCKET)
		{
			Out::println("Cannot create socket!");
			return false;
		}
		else
		{
			Out::println("Socket has been created");
			return true;
		}
	}
	/*Для сервера...*/
	bool bindSocket()
	{
		int result = ::bind(listenSocket, (sockaddr*)&myAddress->getSocketData(), sizeof(sockaddr));
		if (!result)
		{
			Out::println("Socket has been binded on\naddress: %s\nport: %i", myAddress->getIPAddress(), myAddress->getPort());
			thread(&Client::listenEcho, this).detach();
			return true;
		}
		else
		{
			Out::println("Cannot bind socket with\naddress: %s\nport: %i", myAddress->getIPAddress(), myAddress->getPort());
			return false;
		}
	}
	void listenEcho()
	{
		int result = listen(listenSocket, SOMAXCONN);
		if (result) return;
		SOCKET s;
		while (isWorking)
		{
			s = accept(listenSocket, 0, 0);
			if (s == -1) continue;
			char buf[12];
			int res = recv(s, buf, 12, 0);
			if (!strcmp(buf, "echoMessage"))
				send(s, "OK", 3, 0);
			this_thread::sleep_for(chrono::milliseconds(1));
		}
		closesocket(s);
	}
	bool connect()
	{
		int result = ::connect(socket, (sockaddr*)&connectAddress->getSocketData(), sizeof(sockaddr));
		if (!result)
		{
			Out::println("Connected to:\nIP: %s\nPort: %i", connectAddress->getIPAddress(), connectAddress->getPort());
			return true;
		}
		else
		{
			Out::println("Connect failed to:\nIP: %s\nPort: %i", connectAddress->getIPAddress(), connectAddress->getPort());
			return false;
		}
	}
	void sendRequest(Request& req)
	{
		int result = send(socket, (const char*)&req, sizeof(Request), 0);
		Out::println("[Request: %s]: sended %i bytes", req.header, result);
	}
	void setConnectAddress(NetAddress* address)
	{
		connectAddress = address;
	}
	NetAddress* getConnectAddress()
	{
		return connectAddress;
	}
	NetAddress* getClientAddress()
	{
		return myAddress;
	}
};