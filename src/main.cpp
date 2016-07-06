/*
 * main.cpp
 *
 *  Created on: Mar 14, 2016
 *      Author: orgaf
 */

#include <iostream>
#include "TCPMessengerServer.h"

using namespace std;

const char* SERVER_CLI_listAllUsers = "lu";
const char* SERVER_CLI_listAllConnectedUsers = "lcu";
const char* SERVER_CLI_listAllSessions = "ls";
const char* SERVER_CLI_listAllRooms = "lr";
const char* SERVER_CLI_listAllUsersInRoom = "lru";
const char* SERVER_CLI_help = "h";
const char* SERVER_CLI_closeTheApp = "x";

void printMenu()
{
	cout << "options:" << endl;
	printf("%s - prints user names on server\n", SERVER_CLI_listAllUsers);
	printf("%s - prints user names on server\n", SERVER_CLI_listAllUsers);
	printf("%s - prints user names available on server\n", SERVER_CLI_listAllConnectedUsers);
	printf("%s - prints all sessions (two users communicates)\n", SERVER_CLI_listAllSessions);
	printf("%s - print all rooms on server\n", SERVER_CLI_listAllRooms);
	printf("%s - print all users in room\n", SERVER_CLI_listAllUsersInRoom);
	printf("%s	- print this menu\n", SERVER_CLI_help);
	printf("%s	- shutdown\n", SERVER_CLI_closeTheApp);
}

int main()
{
	cout << "welcome to messenger server!" << endl;

	bool isRun = true;
	TCPMessengerServer messengerServer;
	messengerServer.start();
	printMenu();
	string command;

	while (isRun)
	{
		cin >> command;

		if (command == SERVER_CLI_listAllUsers)
		{
			messengerServer.ListAllUsers();
		}
		else if (command == SERVER_CLI_listAllConnectedUsers)
		{
			messengerServer.ListConnectedUsers();
		}
		else if (command == SERVER_CLI_listAllSessions)
		{
			messengerServer.ListAllSessions();
		}
		else if (command == SERVER_CLI_listAllRooms)
		{
			messengerServer.ListAllRooms();
		}
		else if (command == SERVER_CLI_listAllUsersInRoom)
		{
			string room;
			cin >> room;
			messengerServer.ListAllUsersInRoom(room);
		}
		else if (command == SERVER_CLI_help)
		{
			printMenu();
		}
		else if (command == SERVER_CLI_closeTheApp)
		{
			isRun = false;
		}
		else
		{
			cout << "invalid command! type %s for help" << SERVER_CLI_help << endl;
		}
	}

	messengerServer.Close();
	messengerServer.waitForThread();
	cout << "messenger server is closed" << endl;

	return 0;
}
