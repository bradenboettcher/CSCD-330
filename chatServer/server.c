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

struct Room
{
	char name[20];
}Room;

int main(int argc, char ** argv)
{
	printf("::SERVER READY::\n");
	int serverFD, ret;
	int port = atoi(argv[1]);
	struct sockaddr_in serverAddr;
	

	//setup server socket (serverFD)
	serverFD = socket(AF_INET, SOCK_STREAM, 0);
		bzero(&serverAddr, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddr.sin_port = htons(port); //PORT Number given from command line (1300)
		bind(serverFD, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

	//setup OS file-descriptors to monitor (fds)
	fd_set fds, cfds;
	FD_ZERO(&fds);
	FD_SET(serverFD, &fds);


	//Initialize Room Names
	char room1[2] = "X";
	char room2[2] = "Y";
	char room3[2] = "1";

	//Initialize Client List
	Client clientList[10];
	int x = 0;
	for(;x < 10; x++)
	{
		bzero(&clientList[x].name, sizeof(clientList[x].name)+1);
		clientList[x].currentRoom = -1;
		clientList[x].signedIn = 0;
		clientList[x].socket = -1;
	}

	//start running server
	listen(serverFD, 10);
	while(1)
	{
		//reset cfds (select modifies it)
		cfds = fds;
		int i, fd;
		select(14, &cfds, (fd_set*)0,0,0);


		//get packet ready.
		char packet[262173], outgoingPacket[262173];
		char command[1];
		char option[20];
		char size[8];
		char content[262144];

		bzero(&command, sizeof(command));
		bzero(&option, sizeof(option));
		bzero(&size, sizeof(size));
		bzero(&content, sizeof(content));

		for(x = 0; x < 262173; x++)
			packet[x] = '\0';
		

		//if a file descriptor is ready for I/O
		for(i = 0; i < 14; i++)
		{
			//if the serverFD is ready for I/O => We have an incoming client connection.
			if(FD_ISSET(i, &cfds) && i == serverFD)
			{
				int clientSocket = accept(serverFD, (struct sockaddr *)NULL, NULL);
				FD_SET(clientSocket, &fds);

				//CLIENT INIT
				for(x = 0;x < 10; x++)
				{
					if(clientList[x].signedIn == 0)
					{
						clientList[x].signedIn = 1;
						snprintf(clientList[x].name,21,"%s%d","guest",x);
						clientList[x].currentRoom = 0;
						clientList[x].socket = clientSocket;

						printf("%s has joined the server with socket: %d.\n",clientList[x].name,clientSocket);
						break;
					}//end if
				}//end for	
			}//end CLIENT ACCEPT / INITIALIZATION

	
			//if any other file descriptor is ready, read their input & send something back.	
			else if(FD_ISSET(i, &cfds))
			{
				//read the incoming data
				//**NEEDS UPDATE FOR FULL SERVER**

				int n = read(i, packet, 262173);

				//get Current Client Structure
				Client* currentClient;
				for(x = 0; x < 10; x++)
					if(clientList[x].socket == i)
						currentClient = &clientList[x];

				//end get current client

				//IF Client Disconnects
				if(n == 0)
				{
					printf("%s disconnected...\n",currentClient->name);
					bzero(&currentClient->name, sizeof(currentClient->name)+1);
					currentClient->currentRoom = -1;
					currentClient->signedIn = 0;
					currentClient->socket = -1;
					FD_CLR(i,&fds);
					FD_CLR(i,&cfds);
					close(i);				
				}//end client disconnection

				else
				{
					//packet[n] = '\0';
					printf("FROM %s:\n",currentClient->name);

					//THIS IS WHERE YOU SEND THE packet TO THE SWITCH!!!
					switch(packet[0])
					{
						case 'a'	:	//UNUSED
									break;
						case 'b'	:	printf("Broadcast\n");
									for(x = 0;x < 20; x++)
										packet[x+1] = currentClient->name[x];
									for(x = 0; x < 10; x++)
										if(clientList[x].signedIn)
											write(clientList[x].socket,packet,sizeof(packet));

									/*1: use socketID "i" to find sender's name in client list, put it in option field.
									  2: build packet [originalCommand][**sender'sName**][originalSize][originalMessage]
									  3: send new packet to all clients in list (except sender) */
									break;
						case 'c'	:	printf("COMMAND LIST:::\n");
									snprintf(option,sizeof(option)+1,"SERVER");
									snprintf(content,sizeof(content)+1, "--COMMAND LIST--\ncommand - description : usage\n"
													    "/b - Broadcast: /b message to broadcast\n"
													    "/c - Command List: /c\n"
													    "/d - Disconnect: /d\n"
													    "/e - Exit Room and return to general chat: /e\n"
													    "/f - Send File to specific user: /f username FILEPATH\n"
													    "/g - Send File to Room: /f FILEPATH\n"
													    "/h - List of users in current room: /h\n"
													    "/l - List all users in all rooms: /l\n"
													    "/n - Name registration: /n NewName\n"
													    "/r - Send message to current room: /r message\n"
													    "/s - Switch rooms: '/s RoomName' OR just '/s' to see a list of rooms\n"
													    "/w - Whisper: /w nameToWhisperTo message to send\n");

									


									for(x = 1; x < 21; x++)
										packet[x] = option[x-1];
									for(x = 29; x < sizeof(packet); x++)
										packet[x] = content[x-29];
									write(currentClient->socket,packet,sizeof(packet));
									break;
						case 'd'	:	//commands.disconnect(i);
									write(i,packet,262173);
									printf("%s disconnected...\n",currentClient->name);
									bzero(&currentClient->name, sizeof(currentClient->name)+1);
									currentClient->currentRoom = -1;
									currentClient->signedIn = 0;
									currentClient->socket = -1;
									FD_CLR(i,&fds);
									FD_CLR(i,&cfds);
									close(i);									
									break;

						case 'e'	:	//commands.exitRoom(i);
									currentClient->currentRoom = 0;
									snprintf(option, sizeof(option)+1, "SERVER");
									snprintf(content,sizeof(content)+1,"You have exited to the lobby.");
 									for(x = 1; x < 21; x++)
										packet[x] = option[x-1];
									for(x = 29; x < sizeof(packet); x++)
										packet[x] = content[x-29];
									write(currentClient->socket,packet,sizeof(packet));
									break;
						case 'f'	:	//commands.pFile(clinetList,packet);private file
									break;
						case 'g'	:	//group file
									break;
						case 'h'	:	//who's in my room?
									snprintf(option, sizeof(option)+1, "SERVER"); 
									snprintf(content,sizeof(content)+1, "%s", "\nUsers in your room:\n");
									for(x=0;x<10;x++){
										if(clientList[x].name[0]!='\0' && clientList[x].currentRoom==currentClient->currentRoom){
											strcat(content, clientList[x].name);
											strcat(content, "\n");
										}
									}
									
									for(x=1;x<21;x++)
										packet[x]=option[x-1];
									for(x=29;x<sizeof(packet);x++)
										packet[x]=content[x-29];
									write(currentClient->socket,packet,sizeof(packet));
									break;
						case 'i'	:	//UNUSED
									break;
						case 'j'	:	//UNUSED
									break;
						case 'k'	:	//UNUSED
									break;
						case 'l'	:	//list all users in all rooms
									snprintf(option, sizeof(option)+1, "SERVER"); 
									snprintf(content,sizeof(content)+1, "%s", "\nUsers:\n");
									for(x=0;x<10;x++){
										if(clientList[x].name[0]!='\0'){
											strcat(content, clientList[x].name);
											strcat(content, "\n");
										}
									}
									
									for(x=1;x<21;x++)
										packet[x]=option[x-1];
									for(x=29;x<sizeof(packet);x++)
										packet[x]=content[x-29];
									write(currentClient->socket,packet,sizeof(packet));
									break;
						case 'm'	:	//UNUSED
									break;
						case 'n'	:	//name registration
									if(packet[1] == '\0')
									{
										//send error back to client
										//write(currentClient->socket,newpacket,strlen(writes));
									}
									for(x = 1; x < 21; x++)
										currentClient->name[x-1] = packet[x];

									break;
						case 'o'	:	//UNUSED
									break;
						case 'p'	:	//UNUSED
									break;
						case 'q'	:	//UNUSED
									break;
						case 'r'	:	//explicitly send message to room
									printf("Incoming command: %c\n",packet[0]);
									printf("Incoming option: ");
									for(x = 1; x < 21; x++)
										printf("%c",packet[x]);
									printf("\nIncoming size: ");
									for(x = 21; x < 29; x++)
										printf("%c", packet[x]);
									printf("\nIncoming content: ");
									for(x = 29; x < 262173; x++)
										printf("%c", packet[x]);
									printf("\n");
									
									//ALTER PACKET
									for(x = 0;x < 20; x++)
										packet[x+1] = currentClient->name[x];
								
									//DISPLAY OUTGOING PACKET
									printf("Outgoing command: %c\n",packet[0]);
									printf("Outgoing option: ");
									for(x = 1; x < 21; x++)
										printf("%c",packet[x]);
									printf("\nOutgoing size: ");
									for(x = 21; x < 29; x++)
										printf("%c", packet[x]);
									printf("\nOutgoing content: ");
									for(x = 29; x < 262173; x++)
										printf("%c", packet[x]);
									printf("\n");
									
									//WRITE TO OTHERS IN SAME ROOM
									for(x = 0; x < 10; x++)
										if(clientList[x].currentRoom == currentClient->currentRoom)
											write(clientList[x].socket,packet,sizeof(packet));
									break;
						case 's'	:	//switch rooms
									switch(packet[1])
									{
										case 'X' : currentClient->currentRoom = 1;
											   snprintf(option,sizeof(option)+1,"SERVER");
											   snprintf(content,sizeof(content)+1,"You have switched to room 'X'.");
											   break;
										case 'Y' : currentClient->currentRoom = 2;
	  										   snprintf(option,sizeof(option)+1,"SERVER");
											   snprintf(content,sizeof(content)+1,"You have switched to room 'Y'.");
										           break;
										case '1' : currentClient->currentRoom = 3;
	   										   snprintf(option,sizeof(option)+1,"SERVER");
											   snprintf(content,sizeof(content)+1,"You have switched to room '1'.");
											   break;
										default	 : snprintf(option,sizeof(option)+1,"SERVER");
											   snprintf(content,sizeof(content)+1, "Valid rooms are: 'X', 'Y', and '1'.");
									}			
									for(x = 1; x < 21; x++)
										packet[x] = option[x-1];
									for(x = 29; x < sizeof(packet); x++)
										packet[x] = content[x-29];
									write(currentClient->socket,packet,sizeof(packet));
					
									break;
						case 't'	:	//UNUSED
									break;
						case 'u'	:	//UNUSED
									break;
						case 'v'	:	//UNUSED
									break;
						case 'w'	:	//printf("Whisper\n");
									for(x = 1; x < 21; x++)
										option[x-1] = packet[x];
									int sent = 0;
									for(x = 0; x < 10; x++)//for each client in the list
									{
										printf("from: %s\nto: %s",currentClient->name,clientList[x].name);
										if(strcmp(option,clientList[x].name) == 0)
										{
											int y;
											for(y = 1; y < 21; y++)
												packet[y] = currentClient->name[y-1];
											write(clientList[x].socket,packet,sizeof(packet));
											sent = 1;
										}
									}
									if(!sent)
									{
										snprintf(option,sizeof(option)+1,"SERVER");
										snprintf(content,sizeof(content)+1,"User does not exist: type '/l' for a list of current users.");
										for(x = 1; x < 21; x++)
											packet[x] = option[x-1];
										for(x = 29; x < sizeof(packet); x++)
											packet[x] = content[x-29];
										write(currentClient->socket,packet,sizeof(packet));
									}
									break;
						case 'x'	:	//UNUSED
									break;
						case 'y'	:	//UNUSED
									break;
						case 'z'	:	//UNUSED
									break;

						default		:	//DISPLAY INCOMING PACKET
									break;//No Command -> Send to current room								
									
		
					}//end switch
						//write 
						//**NEEDS UPDATE FOR FULL SERVER**
						//write(i,"HELLO!\n", sizeof("HELLO!\n"));

					//CLEAR OUT packet BUFFER
				}//end else (read/write for client)

			}//end if (if a client connects/is ready for i/o)
		}//end for (if a file descriptor is ready for i/o)
	}//end while (keep server running indefinitely)
	return 1;
}//end main





