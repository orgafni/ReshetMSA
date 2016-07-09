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
	friend class Chat;

private:
	PeersRequestsDispatcher* m_dispatcher;
	TCPSocket*				 m_tcpServerSocket;
	bool 					 m_isRunning;

	typedef map<string, SUser*> peers;
	peers m_openedPeers;
	peers m_busyPeers;
	peers m_waitingPeers;

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

	/** ======= Function available from Server CLI: ======= **/
	/**
	 * This function prints all the users.
	 * The users are defines in a file
	 */
	void ListAllUsers();

	/**
	 * This function print out the list of connected clients
	 */
	void ListConnectedUsers();

	/**
	 * List all open sessions between two clients
	 */
	void ListAllSessions();

	/**
	 * List all rooms in the server
	 */
	void ListAllRooms();

	/**
	 * List list all the clients connected to the received room
	 */
	void ListAllUsersInRoom(string roomName);

	/**
	 * This function close the server
	 */
	void Close();

	/** ======== Function called from peers dispatcher ======= **/

	/**
	 * Sign up client to the server
	 */
	void SignUpClient(TCPSocket* client);

	/**
	 * Login signedUp client to the server
	 */
	void LoginClient(TCPSocket* client);

	/**
	 * This function return the names of all the users registered to the server
	 */
	string GetAllUserNames();

	/**
	 * This function return the names of all the users connected to the server
	 */
	string GetAllConnectedUserNames();

	/**
	 * This function return the names of all the rooms users in the server
	 */
	string GetAllRoomNames();

	/**
	 * This function return all the users names connected to the received room
	 */
	string GetAllUsersInRoom(Chat* chat);

	/**
	 * This function connect the received client with his requested client
	 */
	void ConnectClients(TCPSocket* initiatorClient);

	/**
	 * This function create a room, as requested by received peer
	 */
	void CreateNewRoom(TCPSocket* peer);

	/**
	 * This function enter the received peer to the room he request.
	 */
	void EnterRoom(TCPSocket* peer);

	/**
	 * This function disconnect the received peer from the server
	 */
	void Disconnect(TCPSocket* peer);

	/** ======== Utils functions ======= **/

	SUser* GetUserBySocket(TCPSocket* peer);



private:

	/**
	 * return the open peers in a vector
	 */
	vector<TCPSocket*> getPeersVec();

//	/**
//	 * return the open peer that matches the given name (IP:port)
//	 * return NULL if there is no match to the given name
//	 */
//	TCPSocket* getAvailablePeerByName(string peerName);

	/**
	 * remove and delets the given peer
	 */
	void peerDisconnect(TCPSocket* peer);

	/**
	 * move the given peer from the open to the busy peers list
	 */
	void markPeerAsUnavailable(SUser* peer);

	/**
	 * move the given peer from the busy to the open peers list
	 */
	void markPeerAsAvailable(SUser* peer);

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

	SUser* getAvailableUserByName(string userName);


	void closeSession(SUser* user);

	bool closeSessionWithUserIfExist(SUser* user);

	bool closeSessionWithRoomIfExist(SUser* user);

	void sendOpenSessionMsgs(SUser* firstUser, SUser* secondUser);

	void sendCloseSessionMsgs(SUser* initiatorUser, SUser* otherUser);

	void sendLeftRoomMsgs(SUser* userLeft, SUser* otherUser);

	string convertVectorDetailsToString(vector<string> userDetails);

	bool isUserAlreadyConnected(string useName);
};

#endif /* TCPMESSENGERSERVER_H_ */
