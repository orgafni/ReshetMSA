/*
 * PeersRequestsDispatcher.cpp
 *
 *  Created on: Mar 14, 2016
 *      Author: user
 */

#include "PeersRequestsDispatcher.h"
#include "TCPMessengerServer.h"
#include "TCPMessengerProtocol.h"
#include "MultipleTCPSocketListener.h"
#include "Broker.h"
#include "strings.h"

PeersRequestsDispatcher::PeersRequestsDispatcher(TCPMessengerServer* messenger)
{
	m_messenger = messenger;
}

void PeersRequestsDispatcher::run()
{
	cout << "dispatcher started" << endl;

	while (m_messenger->m_isRunning)
	{
		MultipleTCPSocketListener msp;
		msp.addSockets(m_messenger->getPeersVec());
		TCPSocket* readyPeer = msp.listenToSocket(2);
		if (readyPeer == NULL) continue;
		int command = m_messenger->readCommandFromPeer(readyPeer);
		string peerName;
		TCPSocket* secondPeer;

		switch (command)
		{
			case (OPEN_SESSION_WITH_PEER):
			{
				peerName = m_messenger->readDataFromPeer(readyPeer);
				cout << "got open session command: " << readyPeer->GetDestIpAndPort() << "->"  << peerName << endl;
				secondPeer = m_messenger->getAvailablePeerByName(peerName);
				if (secondPeer != NULL)
				{
					m_messenger->sendCommandToPeer(readyPeer, SESSION_ESTABLISHED);
					m_messenger->sendCommandToPeer(secondPeer, OPEN_SESSION_WITH_PEER);
					m_messenger->sendDataToPeer(secondPeer, readyPeer->GetDestIpAndPort());
					m_messenger->markPeerAsUnavailable(secondPeer);
					m_messenger->markPeerAsUnavailable(readyPeer);

					(new Broker(readyPeer, secondPeer, m_messenger))->start();
				}
				else
				{
					cout << "FAIL: didn't find peer: " << peerName << endl;
					m_messenger->sendCommandToPeer(readyPeer, SESSION_REFUSED);
				}
				break;
			}
			default:
			{
				cout << "peer disconnected: " << readyPeer->GetDestIpAndPort() << endl;
				m_messenger->peerDisconnect(readyPeer);
				break;
			}
		}
	}

	cout << "dispatcher ended" << endl;
}


