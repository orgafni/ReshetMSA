/*
 * TCPMessengerServer.cpp
 *
 *  Created on: Mar 14, 2016
 *      Author: user
 */

#include "TCPMessengerServer.h"
#include "TCPMessengerProtocol.h"

TCPMessengerServer::TCPMessengerServer()
{
	m_tcpServerSocket = new TCPSocket(MSNGR_PORT);
	m_isRunning = false;
	m_dispatcher = NULL;
}

void TCPMessengerServer::run()
{
	m_isRunning = true;
	m_dispatcher = new PeersRequestsDispatcher(this);
	m_dispatcher->start();
	while (m_isRunning)
	{
		TCPSocket* peerSocket = m_tcpServerSocket->listenAndAccept();
		if (peerSocket != NULL)
		{
			cout << "new peer connected: " << peerSocket->GetDestIpAndPort() << endl;
			m_openedPeers[peerSocket->GetDestIpAndPort()] = peerSocket;
		}
	}
}

void TCPMessengerServer::ListAllUsers()
{

}

void TCPMessengerServer::ListConnectedUsers()
{
	peers::iterator iter = m_openedPeers.begin();
	peers::iterator endIter = m_openedPeers.end();
	for(;iter != endIter; iter++)
	{
		cout << (*iter).second->GetDestIpAndPort() << endl;
	}
}

void TCPMessengerServer::ListAllRooms()
{

}

void TCPMessengerServer::ListAllUsersInRoom(string room)
{

}

void TCPMessengerServer::Close()
{
	cout << "closing server" << endl;
	m_isRunning = false;
	m_tcpServerSocket->close();
	peers::iterator iter = m_openedPeers.begin();
	peers::iterator endIter = m_openedPeers.end();
	for(;iter != endIter; iter++)
	{
		(*iter).second->close();
	}

	m_dispatcher->waitForThread();
	iter = m_openedPeers.begin();
	endIter = m_openedPeers.end();
	for(;iter != endIter; iter++)
	{
		delete (*iter).second;
	}

	cout << "server closed" << endl;
}

vector<TCPSocket*> TCPMessengerServer::getPeersVec()
{
	vector<TCPSocket*> vec;
	peers::iterator iter = m_openedPeers.begin();
	peers::iterator endIter = m_openedPeers.end();
	for(;iter != endIter; iter++)
	{
		vec.push_back((*iter).second);
	}

	return vec;
}

TCPSocket* TCPMessengerServer::getAvailablePeerByName(string peerName)
{
	peers::iterator iter = m_openedPeers.begin();
	peers::iterator endIter = m_openedPeers.end();
	if (iter == endIter)
	{
		return NULL;
	}
	return (*iter).second;
}

void TCPMessengerServer::peerDisconnect(TCPSocket* peer)
{
	m_openedPeers.erase(peer->GetDestIpAndPort());
	m_busyPeers.erase(peer->GetDestIpAndPort());
	peer->close();
	delete peer;
}

void TCPMessengerServer::markPeerAsUnavailable(TCPSocket* peer)
{
	m_openedPeers.erase(peer->GetDestIpAndPort());
	m_busyPeers[peer->GetDestIpAndPort()] = peer;
}

void TCPMessengerServer::markPeerAsAvailable(TCPSocket* peer)
{
	m_busyPeers.erase(peer->GetDestIpAndPort());
	m_openedPeers[peer->GetDestIpAndPort()] = peer;
}

int TCPMessengerServer::readCommandFromPeer(TCPSocket* peer)
{
	int command = 0;
	int rt = peer->recv((char*)&command, sizeof(command));
	if (rt < 1) return rt;
	command = ntohl(command);
	return command;
}

string TCPMessengerServer::readDataFromPeer(TCPSocket* peer)
{
	string msg;
	char buff[1500];
	int msgLen;
	peer->recv((char*)&msgLen, sizeof(msgLen));
	msgLen = ntohl(msgLen);
	int totalrc = 0;
	int rc;
	while (totalrc < msgLen)
	{
		rc = peer->recv((char*)&buff[totalrc], msgLen - totalrc);
		if (rc > 0)
		{
			totalrc += rc;
		}
		else
		{
			break;
		}
	}
	if (rc > 0 && totalrc == msgLen)
	{
		buff[msgLen] = 0;
		msg = buff;
	}
	else
	{
		peer->close();
	}

	return msg;
}

void TCPMessengerServer::sendCommandToPeer(TCPSocket* peer, int command)
{
	command = htonl(command);
	peer->send((char*)&command, sizeof(command));
}

void TCPMessengerServer::sendDataToPeer(TCPSocket* peer, string msg)
{
	int msgLen = msg.length();
	msgLen = htonl(msgLen);
	peer->send((char*)&msgLen, sizeof(msgLen));
	peer->send(msg.data(), msg.length());
}


