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
const char* SERVER_CLI_listAllRooms = "lr";
const char* SERVER_CLI_listAllUsersInRoom = "lru";
const char* SERVER_CLI_help = "h";
const char* SERVER_CLI_closeTheApp = "x";

void printMenu()
{
	cout << "options:" << endl;
	cout << "%s - prints user names on server" << SERVER_CLI_listAllUsers <<endl;
	cout << "%s - prints user names available on server" << SERVER_CLI_listAllConnectedUsers <<endl;
	cout << "%s - print all rooms on server" << SERVER_CLI_listAllRooms <<endl;
	cout << "%s - print all users in room" << SERVER_CLI_listAllUsersInRoom <<endl;
	cout << "%s	- print this menu" << SERVER_CLI_help <<endl;
	cout << "%s	- shutdown" << SERVER_CLI_closeTheApp <<endl;
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
