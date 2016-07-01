/*
 * MultipleTCPSocketListener.h
 *
 *  Created on: Mar 14, 2016
 *      Author: Or Gafni
 */

#ifndef MULTIPLETCPSOCKETLISTENER_H_
#define MULTIPLETCPSOCKETLISTENER_H_

#include <vector>
#include "TCPSocket.h"

class MultipleTCPSocketListener
{
private:
	vector<TCPSocket*> m_allSockets;
	int m_maxSocket;

public:

	MultipleTCPSocketListener() : m_maxSocket(0){}

	/**
	 * Add the given socket to the socket list to be listen on
	 */
	void addSocket(TCPSocket* socket);

	/**
	 * Set the given sockets to the socket list to be listen on
	 */
	void addSockets(vector<TCPSocket*> socketVec);

	/**
	 * This method checks to see if any of the previous given sockets are ready for reading.
	 * It returns one of the Sockets that are ready.
	 */
	TCPSocket* listenToSocket(int timeout = 0);
};

#endif /* MULTIPLETCPSOCKETLISTENER_H_ */
