/*
 * Broker.h
 *
 *  Created on: May 13, 2016
 *      Author: orgafni
 */

#ifndef BROKER_H_
#define BROKER_H_

#include "MThread.h"
#include "TCPSocket.h"
#include "MultipleTCPSocketListener.h"

// Forward declaration
class TCPMessengerServer;

class Broker: public MThread
{
private:
	TCPSocket* m_firstPeer;
	TCPSocket* m_secondPeer;
	MultipleTCPSocketListener m_listener;
	TCPMessengerServer* const m_server;

public:
	Broker(TCPSocket* first, TCPSocket* second, TCPMessengerServer* const server);

	void run();
};

#endif /* BROKER_H_ */
