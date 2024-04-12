#pragma once
#include <WinSock2.h>
#include <string>

#define String std::string


class AcceptedSocket
{
public:
	SOCKET m_socketFD;
	SOCKADDR_IN m_address;
	int m_error;
	bool m_accepted_successful;
};