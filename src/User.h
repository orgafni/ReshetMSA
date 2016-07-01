/*
 * User.h
 *
 *  Created on: Jul 1, 2016
 *      Author: colman
 */

#ifndef SRC_USER_H_
#define SRC_USER_H_

#include <string>
#include "TCPSocket.h"

using namespace std;

struct SUser
{
	string userName;
	TCPSocket* socket;
	string udpPort;

	SUser(string userName, TCPSocket* socket, string udpPort)
	{
		this->userName = userName;
		this->socket = socket;
		this->udpPort = udpPort;
	}

	string GetUserAddressForChat()
	{
		string userAddress = socket->fromAddr();
		userAddress.append(":");
		userAddress.append(udpPort);

		return userAddress;
	}
};



#endif /* SRC_USER_H_ */
