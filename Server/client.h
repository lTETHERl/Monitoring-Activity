#pragma once
struct Client
{
	SOCKET socket;
	std::string domain;
	std::string machine;
	std::string ip;
	std::string user;
	std::string lastTimeActivity;
};