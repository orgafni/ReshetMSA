/*
 * Chat.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: colman
 */

#include "Chat.h"

#include "TCPMessengerServer.h"

Chat::Chat(SUser* creator, string roomName, TCPMessengerServer* server)
{
	m_roomName = roomName;
	m_creator = creator;
	m_server = server;

	AddUserToRoom(creator);
}


void Chat::AddUserToRoom(SUser* user)
{
	m_connectedClients.push_back(user);

	m_server->markPeerAsUnavailable(user);
}
