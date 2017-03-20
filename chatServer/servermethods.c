#include "servermethods.h"


void buildPacket(char* command, char* option, char* size, char* content, char* packet)
{
	int x = 0;
	for(;x < sizeof(command); x++)
		*packet[x] = *command[x];
	for(; x < sizeof(option)+sizeof(command); x++)
		*packet[x] = *option[x-sizeof(command)];
	for(; x < sizeof(size)+sizeof(option)+sizeof(command); x++)
		*packet[x] = *size[x-(sizeof(option) + sizeof(command))];
	for(; x < sizeof(content)+sizeof(size)+sizeof(option)+sizeof(command); x++)
		*packet[x] = *content[x - (sizeof(size) + sizeof(option) + sizeof(command))];

}// end buildPacket
