/*
 * TCPSocket.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: Or Gafni
 */

#include "TCPSocket.h"
#include "stdio.h"
#include "stdlib.h"
#include <error.h>
#include <errno.h>
#include <unistd.h>

TCPSocket::TCPSocket(int connSock,
					 struct sockaddr_in serverAddr,
					 struct sockaddr_in peerAddr) : m_serAddr(serverAddr),
					 	 	 	 	 	 	 	 	m_prAddr(peerAddr),
					 	 	 	 	 	 	 	 	m_socket_fd(connSock)

{
}


TCPSocket::TCPSocket(int port)
{
	m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_fd < 0)
	{
		perror("Error openning channel");
		::close(m_socket_fd);
		exit(1);
	}

	int enable = 1;
	if (setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
	{
		perror("setsockopt(SO_REUSEADDR) failed");
	}

	bzero((char*)&m_serAddr, sizeof(m_serAddr));
	m_serAddr.sin_family = AF_INET;
	m_serAddr.sin_port = htons(port);

	if (bind(m_socket_fd, (struct sockaddr*)&m_serAddr, sizeof(m_serAddr)) < 0)
	{
		perror("Error naming channel");
		::close(m_socket_fd);
		exit(1);
	}
}

TCPSocket::TCPSocket(string peerIP, int port)
{
	m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_fd < 0)
	{
		perror("Error opening channel");
		::close(m_socket_fd);
		exit(1);
	}

	bzero((char*)&m_prAddr, sizeof(m_prAddr));
	m_prAddr.sin_family = AF_INET;
	if (inet_pton(AF_INET, peerIP.data(), &m_prAddr.sin_addr) != 1)
	{
		perror("fail to convert string ip to int ip");
		::close(m_socket_fd);
		exit(1);
	}
	m_prAddr.sin_port = htons(port);

	if (connect(m_socket_fd, (struct sockaddr*)&m_prAddr, sizeof(m_prAddr)) < 0)
	{
		perror("Error establishing communication");
		::close(m_socket_fd);
		exit(1);
	}

}

TCPSocket* TCPSocket::listenAndAccept()
{
	int rc = listen(m_socket_fd, 1);
	if (rc < 0)
	{
		perror("fail to listen");
		return NULL;
	}

	socklen_t length = sizeof(m_prAddr);
	bzero((char*)&m_prAddr, sizeof(m_prAddr));

	int connectionSock = accept(m_socket_fd, (struct sockaddr*)&m_prAddr, &length);

	return (new TCPSocket(connectionSock, m_serAddr, m_prAddr));
}

int TCPSocket::recv(char* buffer, int length)
{
	return read(m_socket_fd, buffer, length);
}

int TCPSocket::send(const char* msg, int len)
{
	return write(m_socket_fd, msg, len);
}

void TCPSocket::close()
{
	cout<<"closing socket"<<endl;
	shutdown(m_socket_fd,SHUT_RDWR);
	::close(m_socket_fd);
}

string TCPSocket::fromAddr(){
	char fromAddr[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &(m_prAddr.sin_addr), fromAddr, INET_ADDRSTRLEN) == NULL)
	{
		error(1, errno, "fail to convert addr to string");
	}
	return fromAddr;
}

string TCPSocket::GetDestIpAndPort()
{
	string str = fromAddr();
	str.append(":");
	char buff[10];
	sprintf(buff, "%d", ntohs(m_prAddr.sin_port));
	str.append(buff);
	return str;
}

int TCPSocket::getSockFid()
{
	return m_socket_fd;
}
