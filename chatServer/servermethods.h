#ifndef SERVERMETHODS_H
#define SERVERMETHODS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#include <sys/select.h>

typedef struct
{
	char name[20];
	int currentRoom;
	int socket;
	int signedIn;

}Client;

void buildPacket(char* command, char* option, char* size, char* content, char* packet);
