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
		if (readyPeer == NULL)
		{
			continue;
		}
		int command = m_messenger->readCommandFromPeer(readyPeer);
		string peerName;

		switch (command)
		{
			case (SIGNUP):
			{
				m_messenger->SignUpClient(readyPeer);
				break;
			}
			case (LOGIN):
			{
				m_messenger->LoginClient(readyPeer);
				break;
			}
			case (ALL_USERS):
			{
				string usersInServer = m_messenger->GetAllUserNames();

				// If there are not users
				if (usersInServer == "")
				{
					m_messenger->sendCommandToPeer(readyPeer, NO_USERS);
				}
				else
				{
					m_messenger->sendCommandToPeer(readyPeer, ALL_USERS);
					m_messenger->sendDataToPeer(readyPeer, usersInServer);
				}

				break;
			}
			case (OPEN_SESSION_WITH_PEER):
			{
				m_messenger->ConnectClients(readyPeer);
				break;
			}
			case (CONNECTED_USERS):
			{
				string connectedUsersInServer = m_messenger->GetAllConnectedUserNames();

				// If there are not users
				if (connectedUsersInServer == "")
				{
					m_messenger->sendCommandToPeer(readyPeer, NO_USERS);
				}
				else
				{
					m_messenger->sendCommandToPeer(readyPeer, CONNECTED_USERS);
					m_messenger->sendDataToPeer(readyPeer, connectedUsersInServer);
				}

				break;
			}
			case (CLOSE_SESSION_WITH_PEER):
			{
				SUser* user = m_messenger->GetUserBySocket(readyPeer);
				m_messenger->closeSession(user);

				break;
			}
			case (ALL_ROOMS):
			{
				string roomsNames = m_messenger->GetAllRoomNames();

				// If there are not users
				if (roomsNames == "")
				{
					m_messenger->sendCommandToPeer(readyPeer, NO_ROOMS);
				}
				else
				{
					m_messenger->sendCommandToPeer(readyPeer, ALL_ROOMS);
					m_messenger->sendDataToPeer(readyPeer, roomsNames);
				}

				break;
			}
			case (ROOM_USERS):
			{
				// Get roomName from client
				string roomName = m_messenger->readDataFromPeer(readyPeer);

				Chat* chat = m_messenger->getRoomByName(roomName);

				// If no such room found
				if (chat == NULL)
				{
					m_messenger->sendCommandToPeer(readyPeer, ROOM_NOT_EXIST);
				}
				else
				{
					string usersInRoom = m_messenger->GetAllUsersInRoom(chat);

					m_messenger->sendCommandToPeer(readyPeer, ROOM_USERS);
					m_messenger->sendDataToPeer(readyPeer, usersInRoom);
				}

				break;
			}
			case (OPEN_ROOM):
			{
				m_messenger->CreateNewRoom(readyPeer);
				break;
			}
			case (ENTER_ROOM):
			{
				m_messenger->EnterRoom(readyPeer);
				break;
			}
			case (DISCONNECT):
			{
				m_messenger->Disconnect(readyPeer);
				break;
			}
			default:
			{
				SUser* user = m_messenger->GetUserBySocket(readyPeer);
				cout << "the user <" << user->userName << "> send unknown command <" << command << ">" << endl;
				break;
			}

		}
	}

	cout << "dispatcher ended" << endl;
}


