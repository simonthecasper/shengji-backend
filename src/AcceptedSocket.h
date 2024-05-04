#pragma once


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>



class AcceptedSocket
{
public:
	int m_socketFD;
	sockaddr_in m_address;
	int m_error;
	bool m_accepted_successful;

public:
	AcceptedSocket();

	AcceptedSocket(int socketFD, sockaddr_in m_address, int m_error, bool accepted_successful);

	int getSocketFD() const;

	std::string toString() const;
};