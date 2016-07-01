/*
 * PeersRequestsDispatcher.h
 *
 *  Created on: Mar 14, 2016
 *      Author: orgaf
 */

#ifndef PEERSREQUESTSDISPATCHER_H_
#define PEERSREQUESTSDISPATCHER_H_

#include "MThread.h"

// Forward declaration
class TCPMessengerServer;

class PeersRequestsDispatcher : public MThread
{
private:
	TCPMessengerServer* m_messenger;
public:

	/**
	 * Construct that receive a pointer to the parent messenger server
	 */
	PeersRequestsDispatcher(TCPMessengerServer* messenger);

	/**
	 * The Dispatcher main loop
	 */
	void run();
};

#endif /* PEERSREQUESTSDISPATCHER_H_ */
