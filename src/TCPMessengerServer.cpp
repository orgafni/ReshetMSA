/*
 * TCPMessengerServer.cpp
 *
 *  Created on: Mar 14, 2016
 *      Author: user
 */

#include <string.h>
#include "TCPMessengerServer.h"
#include "TCPMessengerProtocol.h"

#define USERS_FILE ("users.txt")

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
			cout << "new user connected. details = " << peerSocket->GetDestIpAndPort() << endl;
			m_waitingPeers[peerSocket->GetDestIpAndPort()] = new SUser("", peerSocket, "");
		}
	}
}

void TCPMessengerServer::ListAllUsers()
{
	string usersNames = GetAllUserNames();

	cout << usersNames;
}

void TCPMessengerServer::ListConnectedUsers()
{
	string usersNames = GetAllConnectedUserNames();

	cout << usersNames;

}

void TCPMessengerServer::ListAllSessions()
{
	cout << " There are " << m_openSessions.size() << " open Sessions" << endl;
	if (m_openSessions.size() != 0)
	{
		map<string, string>::iterator iter = m_openSessions.begin();
		map<string, string>::iterator endIter = m_openSessions.end();
		string firstUser;
		string secondUser;
		for(;iter != endIter; iter++)
		{
			firstUser = m_busyPeers.find((*iter).first)->second->userName;
			secondUser = m_busyPeers.find((*iter).second)->second->userName;
			cout << "session between <" <<  firstUser << "> and <" << secondUser << ">" << endl;
		}
	}
}

void TCPMessengerServer::ListAllRooms()
{
	string roomsNames = GetAllRoomNames();

	cout << " There are " << m_chatRooms.size() << "open chats" << endl;
	cout << roomsNames;

}

void TCPMessengerServer::ListAllUsersInRoom(string room)
{
	Chat* chat = getRoomByName(room);

	if (chat == NULL)
	{
		cout << "room " << room << " not found" << endl;
		return;
	}

	string usersInRoom = GetAllUsersInRoom(chat);
	cout << usersInRoom;

}

void TCPMessengerServer::Close()
{
	cout << "closing server" << endl;
	m_isRunning = false;
	m_tcpServerSocket->close();

	vector<TCPSocket*>socks = getPeersVec();
	vector<TCPSocket*>::iterator iter = socks.begin();
	vector<TCPSocket*>::iterator endIter = socks.end();
//	peers::iterator iter = m_openedPeers.begin();
//	peers::iterator endIter = m_openedPeers.end();
	for(;iter != endIter; iter++)
	{
		Disconnect((*iter));
	}

//	m_dispatcher->waitForThread();
//	iter = m_openedPeers.begin();
//	endIter = m_openedPeers.end();
//	for(;iter != endIter; iter++)
//	{
//		delete (*iter).second;
//	}

	cout << "server closed" << endl;
}

vector<TCPSocket*> TCPMessengerServer::getPeersVec()
{
	vector<TCPSocket*> vec;
	peers::iterator iter = m_openedPeers.begin();
	peers::iterator endIter = m_openedPeers.end();
	for(;iter != endIter; iter++)
	{
		vec.push_back((*iter).second->socket);
	}

	iter = m_busyPeers.begin();
	endIter = m_busyPeers.end();
	for(;iter != endIter; iter++)
	{
		vec.push_back((*iter).second->socket);
	}

	iter = m_waitingPeers.begin();
	endIter = m_waitingPeers.end();
	for(;iter != endIter; iter++)
	{
		vec.push_back((*iter).second->socket);
	}

	return vec;
}

//TCPSocket* TCPMessengerServer::getAvailablePeerByName(string peerName)
//{
//	peers::iterator iter = m_openedPeers.begin();
//	peers::iterator endIter = m_openedPeers.end();
//	if (iter == endIter)
//	{
//		return NULL;
//	}
//	return (*iter).second;
//}

void TCPMessengerServer::peerDisconnect(TCPSocket* peer)
{
	m_openedPeers.erase(peer->GetDestIpAndPort());
	m_busyPeers.erase(peer->GetDestIpAndPort());
	peer->close();
	delete peer;
}

void TCPMessengerServer::markPeerAsUnavailable(SUser* peer)
{
	m_openedPeers.erase(peer->socket->GetDestIpAndPort());
	m_busyPeers[peer->socket->GetDestIpAndPort()] = peer;
}

void TCPMessengerServer::markPeerAsAvailable(SUser* peer)
{
	m_busyPeers.erase(peer->socket->GetDestIpAndPort());
	m_openedPeers[peer->socket->GetDestIpAndPort()] = peer;
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

Chat* TCPMessengerServer::getRoomByName(string roomName)
{
	vector<Chat*>::iterator iter = m_chatRooms.begin();
	vector<Chat*>::iterator iterEnd = m_chatRooms.end();

	for(; iter != iterEnd; iter++)
	{
		if ((*iter)->GetRoomName() == roomName)
		{
			return (*iter);
		}
	}

	return NULL;
}

SUser* TCPMessengerServer::getAvailableUserByName(string userName)
{
	peers::iterator iter = m_openedPeers.begin();
	peers::iterator iterEnd = m_openedPeers.end();

	for(; iter != iterEnd; iter++)
	{
		if ((*iter).second->userName == userName)
		{
			return (*iter).second;
		}
	}

//	iter = m_busyPeers.begin();
//	iterEnd = m_busyPeers.end();
//
//	for(; iter != iterEnd; iter++)
//	{
//		if ((*iter).second->userName() == userName)
//		{
//			return (*iter);
//		}
//	}

	return NULL;
}

SUser* TCPMessengerServer::GetUserBySocket(TCPSocket* peer)
{
	peers::iterator iter = m_openedPeers.begin();
	peers::iterator iterEnd = m_openedPeers.end();

	for(; iter != iterEnd; iter++)
	{
		if ((*iter).first == peer->GetDestIpAndPort())
		{
			return (*iter).second;
		}
	}

	iter = m_busyPeers.begin();
	iterEnd = m_busyPeers.end();

	for(; iter != iterEnd; iter++)
	{
		if ((*iter).first == peer->GetDestIpAndPort())
		{
			return (*iter).second;
		}
	}

	iter = m_waitingPeers.begin();
	iterEnd = m_waitingPeers.end();

	for(; iter != iterEnd; iter++)
	{
		if ((*iter).first == peer->GetDestIpAndPort())
		{
			return (*iter).second;
		}
	}

	return NULL;
}

void TCPMessengerServer::SignUpClient(TCPSocket* clientSocket)
{
	//Read data from client
	string usernamePasswordUdpPort = readDataFromPeer(clientSocket);

	// Split to userName, password, udpPort
	int delimeterIndex = usernamePasswordUdpPort.find(":");
	string userName = usernamePasswordUdpPort.substr(0, delimeterIndex);
	string passwordUdpPort = usernamePasswordUdpPort.substr(delimeterIndex + 1);
	delimeterIndex = passwordUdpPort.find(":");
	string userPassword = passwordUdpPort.substr(0, delimeterIndex);
	string udpPort = passwordUdpPort.substr(delimeterIndex + 1);
//	DEBUG_PRINT("client signup detailsReceived (%s), after parsing (%s:%s:%s)\n", usernamePasswordUdpPort.data(), userName.data(),userPassword.data(), udpPort.data());

	// Check if the username already in the users file
	string user;
	string password;
	bool userAlreadyExists = false;

	// Open the "users" file
	ifstream usersFile;
	usersFile.open(USERS_FILE);

	while (usersFile >> user >> password)
	{
		if (user == userName)
		{
			userAlreadyExists = true;
			break;
		}
	}
	usersFile.close();

	if (userAlreadyExists == true)
	{
		sendCommandToPeer(clientSocket, USER_EXISTS);
	}
	else
	{
		// Add the users to the users file
		ofstream usersFileOut;
		usersFileOut.open(USERS_FILE, ios::app);
		usersFileOut << endl << userName << " " << userPassword;
		usersFile.close();

		// Remove from waiting list
		m_waitingPeers.erase(clientSocket->GetDestIpAndPort());

		// Add the user to the open list
		m_openedPeers[clientSocket->GetDestIpAndPort()] = new SUser(userName, clientSocket, udpPort);
//		DEBUG_PRINT("client signup. new user created with (%s:%s)\n", userName.data(), udpPort.data());

		sendCommandToPeer(clientSocket, SIGNUP_OK);
	}



}

void TCPMessengerServer::LoginClient(TCPSocket* clientSocket)
{
	//Read data from client
	string usernamePasswordUdpPort = readDataFromPeer(clientSocket);

	// Split to userName, password, udpPort
	int delimeterIndex = usernamePasswordUdpPort.find(":");
	string userName = usernamePasswordUdpPort.substr(0, delimeterIndex);
	string passwordUdpPort = usernamePasswordUdpPort.substr(delimeterIndex + 1);
	delimeterIndex = passwordUdpPort.find(":");
	string userPassword = passwordUdpPort.substr(0, delimeterIndex);
	string udpPort = passwordUdpPort.substr(delimeterIndex + 1);
//	DEBUG_PRINT("client login detailsReceived (%s), after parsing (%s:%s:%s)\n", usernamePasswordUdpPort.data(), userName.data(),userPassword.data(), udpPort.data());

	// Verify that the user exist in the users file, and the password is correct
	string user;
	string password;
	bool userFoundAndOK = false;

	// Open the "users" file
	ifstream usersFile;
	usersFile.open(USERS_FILE);

	while (usersFile >> user >> password)
	{
		if (user == userName)
		{
			if (userPassword == password)
			{
				userFoundAndOK = true;
			}
			break;
		}
	}
	usersFile.close();

	if (userFoundAndOK == false)
	{
		sendCommandToPeer(clientSocket, CONNECTION_REFUSED);
	}
	else
	{
		// Verify that the username requested not already connected
		if (isUserAlreadyConnected(userName) == true)
		{
			sendCommandToPeer(clientSocket, CONNECTION_REFUSED);
		}
		else
		{
			// Remove from waiting list
			m_waitingPeers.erase(clientSocket->GetDestIpAndPort());

	//		DEBUG_PRINT("client login. new user created with (%s:%s)\n", userName.data(), udpPort.data());
			// Add the user to the open list
			m_openedPeers[clientSocket->GetDestIpAndPort()] = new SUser(userName, clientSocket, udpPort);

			sendCommandToPeer(clientSocket, CONNECTION_VALID);
		}
	}

}

string TCPMessengerServer::GetAllConnectedUserNames()
{
	peers::iterator iter = m_openedPeers.begin();
	peers::iterator endIter = m_openedPeers.end();

	string connectedUserNames = "";
	for(;iter != endIter; iter++)
	{
		connectedUserNames.append((*iter).second->userName);
		connectedUserNames.append("\n");
	}

	iter = m_busyPeers.begin();
	endIter = m_busyPeers.end();
	for(;iter != endIter; iter++)
	{
		connectedUserNames.append((*iter).second->userName);
		connectedUserNames.append("\n");
	}

	return connectedUserNames;
}

string TCPMessengerServer::GetAllUserNames()
{
	string user;
	string password;

	// Open the "users" file
	ifstream usersFile;
	usersFile.open(USERS_FILE);

	string allUsersNames = "";
	while (usersFile >> user >> password)
	{
		allUsersNames.append(user);
		allUsersNames.append("\n");
	}

	usersFile.close();

	return allUsersNames;
}

string TCPMessengerServer::GetAllRoomNames()
{
	string roomsNames = "";
	if (m_chatRooms.size() != 0)
	{
		vector<Chat*>::iterator iter = m_chatRooms.begin();
		vector<Chat*>::iterator endIter = m_chatRooms.end();
		for(;iter != endIter; iter++)
		{
			roomsNames.append((*iter)->GetRoomName());
			roomsNames.append("\n");
		}
	}

	return roomsNames;
}

string TCPMessengerServer::GetAllUsersInRoom(Chat* chat)
{
	vector<SUser*>::iterator iter = chat->m_connectedClients.begin();
	vector<SUser*>::iterator iterEnd = chat->m_connectedClients.end();

	string usersInChat = "";

	for(;iter != iterEnd; iter++)
	{
		usersInChat.append((*iter)->userName);
		usersInChat.append("\n");
	}

	return usersInChat;
}

void TCPMessengerServer::ConnectClients(TCPSocket* initiatorClient)
{
	string requestedUserName = readDataFromPeer(initiatorClient);

	// Get the client that the initiator want to talk to
	SUser* requestedUser = getAvailableUserByName(requestedUserName);

	// If there is no user with the requested userName, or the user isn't available
	if (requestedUser == NULL)
	{
		sendCommandToPeer(initiatorClient, USER_NO_EXIST);
	}
	else
	{
		SUser* initiatorUser = GetUserBySocket(initiatorClient);
		if (initiatorUser == NULL)
		{
			return;
		}

		// Close sessions the initiator already in
		closeSession(initiatorUser);

		// Notify both user about the establishment of the session
		sendOpenSessionMsgs(initiatorUser, requestedUser);

		// Set both users as unavailable
		markPeerAsUnavailable(initiatorUser);
		markPeerAsUnavailable(requestedUser);

		m_openSessions[initiatorUser->socket->GetDestIpAndPort()] = requestedUser->socket->GetDestIpAndPort();
	}
}

void TCPMessengerServer::CreateNewRoom(TCPSocket* peer)
{
	// Get room name from client
	string roomName = readDataFromPeer(peer);

	// Verify the room is not exist already
	if (getRoomByName(roomName) != NULL)
	{
		sendCommandToPeer(peer, ROOM_EXIST);
		sendDataToPeer(peer, roomName);
	}
	else
	{
		SUser* user = GetUserBySocket(peer);

		// Close user other sessions
		closeSession(user);

		// Create a room
		Chat* chat = new Chat(user, roomName, this);
		m_chatRooms.push_back(chat);

		// Notify the user that his room created
		sendCommandToPeer(peer, OPEN_ROOM);
		sendDataToPeer(peer, roomName);
	}
}

void TCPMessengerServer::EnterRoom(TCPSocket* peer)
{
	// Get room name from client
	string roomName = readDataFromPeer(peer);

	Chat* chat = getRoomByName(roomName);
	// Verify the room is exist already
	if (chat == NULL)
	{
		sendCommandToPeer(peer, ROOM_NOT_EXIST);
		sendDataToPeer(peer, roomName);
	}
	else
	{
		SUser* user = GetUserBySocket(peer);

		// Close user other sessions
		closeSession(user);

		// Create the new user details
		vector<string> newUserDetails;
		newUserDetails.push_back(user->userName);
		newUserDetails.push_back(user->GetUserAddressForChat());
		string newUserDetailsString = convertVectorDetailsToString(newUserDetails);

		// Create all users in room details
		vector<string> allUsersDetails;
		allUsersDetails.push_back(roomName);

		vector<SUser*>::iterator userIter = chat->m_connectedClients.begin();
		vector<SUser*>::iterator userIterEnd = chat->m_connectedClients.end();

		// Send to all the users in the room that a new user is entered their room.
		// In addition, collect the users details, in order to send to the new user
		for (; userIter != userIterEnd; userIter++)
		{
			sendCommandToPeer((*userIter)->socket, ENTER_ROOM);
			sendDataToPeer((*userIter)->socket, newUserDetailsString);

			allUsersDetails.push_back((*userIter)->userName);
			allUsersDetails.push_back((*userIter)->GetUserAddressForChat());
		}

		string allUserDetailsString = convertVectorDetailsToString(allUsersDetails);

		// Add the new user to the room
		chat->AddUserToRoom(user);
		cout << "The user  <" << user->userName << "> entered the room <" << roomName << ">" << endl;

		// Notify the user that he entered the room. and send him all the users in the room details
		sendCommandToPeer(peer, CONNECTED_TO_ROOM);
		sendDataToPeer(peer, allUserDetailsString);
	}
}

void TCPMessengerServer::Disconnect(TCPSocket* peer)
{
	SUser* user = GetUserBySocket(peer);

	// Close the peer from all its sessions
	closeSession(user);

	// Delete user form the openedPeers and from waitingPeers if exist (not going to be in the busyPeers go he got no sessions)
	peers::iterator userIter = m_openedPeers.find(peer->GetDestIpAndPort());

	// Just in case the user found
	if (userIter != m_openedPeers.end())
	{
		delete userIter->second;
		m_openedPeers.erase(userIter);
	}

	userIter = m_waitingPeers.find(peer->GetDestIpAndPort());

	// Just in case the user found
	if (userIter != m_waitingPeers.end())
	{
		delete userIter->second;
		m_waitingPeers.erase(userIter);
	}

	// Notify the client the he disconnected successfully
	sendCommandToPeer(peer, DISCONNECT);
	peer->close();
	delete peer;
}

void TCPMessengerServer::closeSession(SUser* user)
{
	cout << " closing user: " << user->userName << " session." << endl;

	bool sessionFound = closeSessionWithUserIfExist(user);

	// If we can't found a session with user for our peer, check if he is in room
	if (sessionFound == false)
	{
		sessionFound = closeSessionWithRoomIfExist(user);
	}

	if (sessionFound == true)
	{
		cout << "session closed successfully" << endl;
	}
	else
	{
		cout << "session for <" << user->userName << "> not found" << endl;
	}

}

bool TCPMessengerServer::closeSessionWithUserIfExist(SUser* user)
{
	bool sessionFound = false;

	map<string, string>::iterator iter = m_openSessions.begin();
	map<string, string>::iterator iterEnd = m_openSessions.end();

	SUser* otherUser = NULL;

	// Scan all the open sessions, and find the otherUser that our user is connected to.
	for(;iter != iterEnd; iter++)
	{
		// In case our user is the first user in the session
		if (iter->first == user->socket->GetDestIpAndPort())
		{
			otherUser = m_busyPeers.find(iter->second)->second;
		}
		// In case our user is the second user in the session
		else if (iter->second == user->socket->GetDestIpAndPort())
		{
			otherUser = m_busyPeers.find(iter->first)->second;
		}

		// If we found the otherUser
		if (otherUser != NULL)
		{
			cout << "closing session with " << otherUser->userName << endl;

			// Set both users as available
			markPeerAsAvailable(user);
			markPeerAsAvailable(otherUser);

			// Delete the session
			m_openSessions.erase(iter);

			sendCloseSessionMsgs(user, otherUser);
			sessionFound = true;
			break;

		}

	}

	return sessionFound;
}

bool TCPMessengerServer::closeSessionWithRoomIfExist(SUser* user)
{
	bool sessionFound = false;

	vector<Chat*>::iterator roomIter = m_chatRooms.begin();
	vector<Chat*>::iterator roomIterEnd = m_chatRooms.end();

	vector<SUser*>::iterator userIter;
	vector<SUser*>::iterator userIterEnd;

	bool isUserFound = false;
	// Scan all the rooms in the server
	for (; roomIter != roomIterEnd; roomIter++)
	{
		userIter = (*roomIter)->m_connectedClients.begin();
		userIterEnd = (*roomIter)->m_connectedClients.end();

		// Scan all the users in the current room
		for (;userIter != userIterEnd; userIter++)
		{
			if ((*userIter)->userName == user->userName)
			{
				(*roomIter)->m_connectedClients.erase(userIter);

				cout << "The user <" << user->userName << "> left the room <" << (*roomIter)->GetRoomName() << ">" << endl;

				isUserFound = true;
			}
		}

		if (isUserFound == true)
		{
			// Tell all the clients about the user left the room
			userIter = (*roomIter)->m_connectedClients.begin();
			userIterEnd = (*roomIter)->m_connectedClients.end();

			for (;userIter != userIterEnd; userIter++)
			{
				sendLeftRoomMsgs(user, (*userIter));
			}

			// Notify the userLeft that he left the room
			sendCommandToPeer(user->socket, CLOSE_SESSION_WITH_ROOM);
			sendDataToPeer(user->socket, "me");

			sessionFound = true;
			markPeerAsAvailable(user);

			// User can be only in one room, no need to continue search him.
			break;
		}
	}

	return sessionFound;
}

void TCPMessengerServer::sendOpenSessionMsgs(SUser* firstUser, SUser* secondUser)
{
	vector<string> firstUserDetails;
	firstUserDetails.push_back(firstUser->userName);
	firstUserDetails.push_back(firstUser->GetUserAddressForChat());

	string firstStringDetails = convertVectorDetailsToString(firstUserDetails);

	// Notify the second user that the first user open session with him
	sendCommandToPeer(secondUser->socket, SESSION_ESTABLISHED);
	sendDataToPeer(secondUser->socket, firstStringDetails);
//	DEBUG_PRINT("sending first userDetails (%s) to second user (%s)\n", firstStringDetails.data(), secondUser->userName.data());

	vector<string> secondUserDetails;
	secondUserDetails.push_back(secondUser->userName);
	secondUserDetails.push_back(secondUser->GetUserAddressForChat());

	string secondStringDetails = convertVectorDetailsToString(secondUserDetails);

	// Notify the first user that the second user open session with him
	sendCommandToPeer(firstUser->socket, SESSION_ESTABLISHED);
	sendDataToPeer(firstUser->socket, secondStringDetails);
//	DEBUG_PRINT("sending second userDetails (%s) to first user (%s)\n", secondStringDetails.data(), firstUser->userName.data());
}

void TCPMessengerServer::sendCloseSessionMsgs(SUser* initiatorUser, SUser* otherUser)
{
	vector<string> initiatorUserDetails;
	initiatorUserDetails.push_back(initiatorUser->userName);
	initiatorUserDetails.push_back(initiatorUser->GetUserAddressForChat());

	string stringDetails = convertVectorDetailsToString(initiatorUserDetails);

	// Notify the otherUser the the initiatorUser closed their session
	sendCommandToPeer(otherUser->socket, CLOSE_SESSION_WITH_PEER);
	sendDataToPeer(otherUser->socket, stringDetails);

	// Notify the initiatorUser that he close his session
	sendCommandToPeer(initiatorUser->socket, CLOSE_SESSION_WITH_PEER);
	sendDataToPeer(initiatorUser->socket, "me");
}

void TCPMessengerServer::sendLeftRoomMsgs(SUser* userLeft, SUser* otherUser)
{
	vector<string> userLeftDetails;
	userLeftDetails.push_back(userLeft->userName);
	userLeftDetails.push_back(userLeft->GetUserAddressForChat());

	string stringDetails = convertVectorDetailsToString(userLeftDetails);

	// Notify the otherUser that userLeft left the room
	sendCommandToPeer(otherUser->socket, CLOSE_SESSION_WITH_ROOM);
	sendDataToPeer(otherUser->socket, stringDetails);
}

string TCPMessengerServer::convertVectorDetailsToString(vector<string> userDetails)
{
	char stringDetails[999] = "";

	if (userDetails.size() != 0)
	{
		vector<string>::iterator detailIter = userDetails.begin();
		strcpy(stringDetails, detailIter->data());

		if (userDetails.size() != 1)
		{
			detailIter++;
			for (; detailIter != userDetails.end(); detailIter++)
			{
				strcat(stringDetails, ":");
				strcat(stringDetails, detailIter->data());
			}
		}
	}

	return stringDetails;
}

bool TCPMessengerServer::isUserAlreadyConnected(string userName)
{
	peers::iterator iter = m_openedPeers.begin();
	peers::iterator endIter = m_openedPeers.end();

	for(;iter != endIter; iter++)
	{
		if ((*iter).second->userName == userName)
		{
			return true;
		}
	}

	iter = m_busyPeers.begin();
	endIter = m_busyPeers.end();
	for(;iter != endIter; iter++)
	{
		if ((*iter).second->userName == userName)
		{
			return true;
		}
	}

	return false;
}
