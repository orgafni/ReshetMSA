/*
 * Chat.h
 *
 *  Created on: Jul 1, 2016
 *      Author: colman
 */

#ifndef SRC_CHAT_H_
#define SRC_CHAT_H_

#include <string>
#include <vector>
#include "User.h"

class TCPMessengerServer;

using namespace std;

class Chat
{
	friend class TCPMessengerServer;

public:
	Chat(SUser* creator, string roomName, TCPMessengerServer* server);

	string GetRoomName()
	{
		return m_roomName;
	}

	string GetCreatorName()
	{
		return m_creator->userName;
	}

	void AddUserToRoom(SUser* user);

private:
	string m_roomName;
	SUser* m_creator;
	TCPMessengerServer* m_server;
	vector<SUser*> m_connectedClients;
};

#endif /* SRC_CHAT_H_ */
