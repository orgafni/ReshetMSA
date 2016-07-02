/*
 * TCPMessengerProtocol.h
 *
 *  Created on: Feb 13, 2013
 *      Author: efi
 */

#ifndef TCPMESSENGERPROTOCOL_H_
#define TCPMESSENGERPROTOCOL_H_

/**
 * TCP Messenger protocol:
 * all messages are of the format [Command 4 byte int]
 * and optionally data follows in the format [Data length 4 byte int][ Data ]
 */
#define MSNGR_PORT 1234

enum ECommands
{
	CLOSE_SESSION_WITH_PEER 	= 1,
	CLOSE_SESSION_WITH_ROOM,
	OPEN_SESSION_WITH_PEER,
	EXIT,
	SESSION_REFUSED,
	SESSION_ESTABLISHED,
	CONNECTION_REFUSED,
	CONNECTION_VALID,
	USER_EXISTS,
	SIGNUP_OK,
	SIGNUP,
	LOGIN,
	ALL_USERS,
	CONNECTED_USERS,
	ROOM_USERS,
	ALL_ROOMS,
	ROOM_NOT_EXIST,
	CONNECTED_TO_ROOM,
	OPEN_ROOM,
	ROOM_EXIST,
	DISCONNECT,
	GET_USER_DETAILS,
	USER_NO_EXIST,
	NO_ROOMS,
	NO_USERS,
	ENTER_ROOM
};

#define TEST_PEER_NAME "test"
#define SESSION_REFUSED_MSG "Connection to peer refused, peer might be busy or disconnected, try again later"



#endif /* TCPMESSENGERPROTOCOL_H_ */
