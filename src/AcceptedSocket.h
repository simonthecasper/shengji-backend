#pragma once


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>



class AcceptedSocket
{
public:
	SOCKET m_socketFD;
	SOCKADDR_IN m_address;
	SOCKET m_error;
	bool m_accepted_successful;

public:

	AcceptedSocket();

	AcceptedSocket(SOCKET socketFD, SOCKADDR_IN m_address, SOCKET m_error, bool accepted_successful);

	SOCKET getSocketFD() const;

	std::string toString() const;
};