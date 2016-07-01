/*
 * TCPSocket.h
 *
 *  Created on: Mar 13, 2016
 *      Author: Or Gafni
 */

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

#include <netinet/in.h>
#include <inttypes.h>
#include <strings.h>
#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>


using namespace std;

class TCPSocket
{
private:

	struct sockaddr_in m_serAddr;
	struct sockaddr_in m_prAddr;
	int 			   m_socket_fd;

	/**
	 * Private constructor to create a secondary server socket to communicate with a remote peer.
	 */
	TCPSocket(int connSock, struct sockaddr_in serverAddr, struct sockaddr_in peerAddr);


public:

	/**
	 * Constructor to create a TCP  server socket
	 */
	TCPSocket(int port);

	/**
	 * Constructor to create a TCP client socket
	 */
	TCPSocket(string peerIP, int port);

	/**
	 * Perform a listen and accept on a server socket
	 *
	 * @return a TCP socket to communicate over the incoming session
	 */
	TCPSocket* listenAndAccept();

	/**
	 * Read from socket into the given buffer up to the buffer given length.
	 *
	 * @return the number of bytes read
	 */
	int recv(char* buffer, int length);

	/**
	 * Send the given buffer to the socket.
	 */
	int send(const char* msg, int len);

	/**
	 * close the socket and free all its resources
	 */
	void close();

	/**
	 * return the address of the connected peer.
	 */
	string fromAddr();

	string GetDestIpAndPort();

	int getSockFid();

};

#endif /* TCPSOCKET_H_ */
