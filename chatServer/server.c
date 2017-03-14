#include <stdio.h>
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
	char currentRoom[20];
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



	//Initialize Client List
	Client clientList[10];
	int x = 0;
	for(;x < 10; x++)
	{
		bzero(&clientList[x].name, sizeof(clientList[x].name)+1);
		bzero(&clientList[x].currentRoom, sizeof(clientList[x].currentRoom)+1);
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
		select(10, &cfds, (fd_set*)0,0,0);

		//if a file descriptor is ready for I/O
		for(i = 0; i < 10; i++)
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
						snprintf(clientList[x].currentRoom,21,"%s","General");
						clientList[x].socket = clientSocket;

						printf("%s has joined the server.\n",clientList[x].name);
						x = 11;
					}//end if
				}//end for	
			}//end CLIENT ACCEPT / INITIALIZATION

	
			//if any other file descriptor is ready, read their input & send something back.	
			else if(FD_ISSET(i, &cfds))
			{
				//read the incoming data
				//**NEEDS UPDATE FOR FULL SERVER**

				
				char packet[1000];
				int n = read(i, packet, sizeof(packet));

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
					bzero(&currentClient->currentRoom, sizeof(currentClient->currentRoom)+1);
					currentClient->signedIn = 0;
					currentClient->socket = -1;
					FD_CLR(i,&fds);				
				}//end client disconnection

				else
				{
					packet[n] = '\0';
					printf("%s: ",currentClient->name);

					//THIS IS WHERE YOU SEND THE PACKET TO THE SWITCH!!!
					switch(packet[0])
					{
						case 'a'	:	//UNUSED
									break;
						case 'b'	:	printf("Broadcast\n");
									/*1: use socketID "i" to find sender's name in client list, put it in option field.
									  2: build packet [originalCommand][**sender'sName**][originalSize][originalMessage]
									  3: send new packet to all clients in list (except sender) */
									break;
						case 'c'	:	//command list//commands.commandList(i);
									break;
						case 'd'	:	//commands.disconnect(i);
									break;
						case 'e'	:	//commands.exitRoom(i);
									break;
						case 'f'	:	//commands.pFile(clinetList,packet);private file
									break;
						case 'g'	:	//group file
									break;
						case 'h'	:	//who's in my room?
									break;
						case 'i'	:	//UNUSED
									break;
						case 'j'	:	//UNUSED
									break;
						case 'k'	:	//UNUSED
									break;
						case 'l'	:	//list all users in all rooms
									break;
						case 'm'	:	//UNUSED
									break;
						case 'n'	:	//name registration
									



									break;
						case 'o'	:	//UNUSED
									break;
						case 'p'	:	//UNUSED
									break;
						case 'q'	:	//UNUSED
									break;
						case 'r'	:	//explicitly send message to room
									break;
						case 's'	:	//switch rooms
									break;
						case 't'	:	//UNUSED
									break;
						case 'u'	:	//UNUSED
									break;
						case 'v'	:	//UNUSED
									break;
						case 'w'	:	printf("Whisper\n");//Whisper
									break;
						case 'x'	:	//UNUSED
									break;
						case 'y'	:	//UNUSED
									break;
						case 'z'	:	//UNUSED
									break;
						default		:	break;//No Command -> Send to current room
		
					}//end switch




						for(x = 0; x < 284; x++)
							printf("%c",packet[x]);
						//write 
						//**NEEDS UPDATE FOR FULL SERVER**
						//write(i,"HELLO!\n", sizeof("HELLO!\n"));

					//CLEAR OUT PACKET BUFFER
					for(x = 0; x < 284; x++)
						packet[x] = '\0';
				}

			}//end if
		}//end for
	}//end while (keep server running indefinitely)

	//fd_set fds, cfds;
	//FD_ZERO(&fds);
	//long int [256] array;

	//FD_SET(serverfd, &fds);

	//listen(serverfd, 8);
	//for(;;)
	//{
	//	cfds = fds;
	//	FD_SET(accept(serverfd, (struct sockaddr *) NULL, NULL), &fds)
	//	select(10, &cfds, (fd.set*)0, 0, 0);
	//	
	//	
	//
	//
	//}//end for
	return;
}//end main





