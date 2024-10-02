#pragma once
#include <Windows.h>
#include <vector>
#include <thread>
#include <ctime>
#include <sstream>
#include <mutex>
#include <string>
#include <fstream>
#include <commctrl.h>
#include "client.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "comctl32.lib")


class Server
{
public:
	Server();
	~Server();
	std::vector<std::shared_ptr<Client>> getClients();
	void makeScreenshot(int);

	void startServer();

private:
	static WSADATA wsaData;
	static SOCKET serverSocket;
	static sockaddr_in serverAddr;

	std::vector<std::shared_ptr<Client>> clients;

	void ping(SOCKET&);
	void checkClient(Client&);

	std::string getCurrentTime();
	void printClients();

	void receiveScreenshot(SOCKET& clientSocket);
};