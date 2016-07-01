/*
 * TCPMessengerServer.h
 *
 *  Created on: Mar 14, 2016
 *      Author: orgaf
 */

#ifndef TCPMESSENGERSERVER_H_
#define TCPMESSENGERSERVER_H_

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include "MThread.h"
#include "PeersRequestsDispatcher.h"
#include "TCPSocket.h"
#include "User.h"
#include "Chat.h"

using namespace std;

class TCPMessengerServer : public MThread
{
	friend class PeersRequestsDispatcher;
	friend class Broker;

private:
	PeersRequestsDispatcher* m_dispatcher;
	TCPSocket*				 m_tcpServerSocket;
	bool 					 m_isRunning;

	typedef map<string, SUser*> peers;
	peers m_openedPeers;
	peers m_busyPeers;

	vector<Chat*> m_chatRooms;

	map <string, string> m_openSessions;

public:
	TCPMessengerServer();

	/**
	 * The messenger sever main loop - this loop waits for incoming clients connection,
	 * once a connection is established, the new connection is added to the m_openedPeers
	 * on which the dispatcher handle further communication
	 */
	void run();

	/**
	 * This function prints all the users.
	 * The users are defines in a file
	 */
	void ListAllUsers();

	/**
	 * This function print out the list of connected clients
	 */
	void ListConnectedUsers();

	void ListAllSessions();

	void ListAllRooms();

	void ListAllUsersInRoom(string room);

	/**
	 * This function close the server
	 */
	void Close();

private:

	/**
	 * return the open peers in a vector
	 */
	vector<TCPSocket*> getPeersVec();

	/**
	 * return the open peer that matches the given name (IP:port)
	 * return NULL if there is no match to the given name
	 */
	TCPSocket* getAvailablePeerByName(string peerName);

	/**
	 * remove and delets the given peer
	 */
	void peerDisconnect(TCPSocket* peer);

	/**
	 * move the given peer from the open to the busy peers list
	 */
	void markPeerAsUnavailable(TCPSocket* peer);

	/**
	 * move the given peer from the busy to the open peers list
	 */
	void markPeerAsAvailable(TCPSocket* peer);

	/**
	 * read command from peer
	 */
	int readCommandFromPeer(TCPSocket* peer);

	/**
	 * read data from peer
	 */
	string readDataFromPeer(TCPSocket* peer);

	/**
	 * send command to peer
	 */
	void sendCommandToPeer(TCPSocket* peer, int command);

	/**
	 * send data to peer
	 */
	void sendDataToPeer(TCPSocket* peer, string msg);

	Chat* getRoomByName(string roomName);
};

#endif /* TCPMESSENGERSERVER_H_ */
