/*
 * Chat.h
 *
 *  Created on: Jul 1, 2016
 *      Author: colman
 */

#ifndef SRC_CHAT_H_
#define SRC_CHAT_H_

#include <string>

class TCPMessengerServer;

using namespace std;

class Chat
{
	friend class TCPMessengerServer;

public:
	Chat(){};

	string GetRoomName()
	{
		return m_roomName;
	}

private:
	string m_roomName;
	vector<SUser*> m_connectedClients;
};

#endif /* SRC_CHAT_H_ */
