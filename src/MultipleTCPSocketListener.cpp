/*
 * MultipleTCPSocketListener.cpp
 *
 *  Created on: Mar 14, 2016
 *      Author: user
 */

#include "MultipleTCPSocketListener.h"

void MultipleTCPSocketListener::addSocket(TCPSocket* socket)
{
	// Add the given socket to the socket list
	m_allSockets.push_back(socket);

	if (socket->getSockFid() > m_maxSocket)
	{
		m_maxSocket = socket->getSockFid();
	}
}

void MultipleTCPSocketListener::addSockets(vector<TCPSocket*> socketVec)
{
	for (vector<TCPSocket*>::iterator sock = socketVec.begin(); sock != socketVec.end(); ++sock)
	{
		addSocket(*sock);
	}
}

TCPSocket* MultipleTCPSocketListener::listenToSocket(int timeout)
{
	// Create local set for the select function (fd_set)
	fd_set set;
	FD_ZERO(&set);

	// Fill the set with the file descriptors from the socket list using (FD_SET macro)
	for (vector<TCPSocket*>::iterator sock = m_allSockets.begin(); sock != m_allSockets.end(); ++sock)
	{
		FD_SET((*sock)->getSockFid(), &set);
	}

	struct timeval tv = {timeout, 0};

	struct timeval *pTv = NULL;
	if (timeout > 0)
	{
		pTv = &tv;
	}

	// preform the select
	int val;
	val = select(m_maxSocket + 1, &set, NULL, NULL, pTv);

	// Check the returned value from the select to find the socket that is ready
	if (val == -1)
	{
		perror("error in the select");
		return NULL;
	}
	else if (val == 0)
	{
		return NULL;
	}

	// If select return a valid socket, return the matching TCPSocket object, otherwise return NULL
	for (vector<TCPSocket*>::iterator sock = m_allSockets.begin(); sock != m_allSockets.end(); ++sock)
	{
		if (FD_ISSET((*sock)->getSockFid(), &set))
		{
			return *sock;
		}
	}

	return NULL;
}
