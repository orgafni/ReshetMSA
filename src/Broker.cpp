/*
 * Broker.cpp
 *
 *  Created on: May 13, 2016
 *      Author: orgafni
 */

#include "Broker.h"
#include "TCPMessengerProtocol.h"
#include "TCPMessengerServer.h"

Broker::Broker(TCPSocket* first, TCPSocket* second, TCPMessengerServer* const server) : m_firstPeer(first),
																						m_secondPeer(second),
																						m_listener(),
																						m_server(server)
{
	m_listener.addSocket(m_firstPeer);
	m_listener.addSocket(m_secondPeer);
}

void Broker::run()
{
	bool shouldRun = true;

	while(shouldRun)
	{
		// Listen to sockets
		TCPSocket* returned = m_listener.listenToSocket(0);

		if (returned == NULL)
		{
			break;
		}

		// Read from socket
		int command = m_server->readCommandFromPeer(returned);

		// Set peer socket
		TCPSocket* peer = returned->getSockFid() == m_firstPeer->getSockFid() ? m_secondPeer : m_firstPeer;

		switch(command)
		{
			case (CLOSE_SESSION_WITH_PEER):
			{
				// Send peer close command
				m_server->sendCommandToPeer(peer, CLOSE_SESSION_WITH_PEER);

				// Mark peers as available
				m_server->markPeerAsAvailable(peer);
				m_server->markPeerAsAvailable(returned);
				shouldRun = false;
				delete this;
				break;
			}
			case (SEND_MSG_TO_PEER):
			{
				m_server->sendCommandToPeer(peer, SEND_MSG_TO_PEER);
				m_server->sendDataToPeer(peer, m_server->readDataFromPeer(returned));
				break;
			}
		}
	}
}
