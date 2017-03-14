#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool threadFinished = 0;

void * readThread(int * sockfd)
{
	int ret;
	while(1)
	{
		char reads[256];
		ret = read(*sockfd,reads,sizeof(reads));
		if(ret > 0)
		{
			reads[ret] = '\0';
			printf("SERVER: %s",reads);
		}
		else if(ret == 0)
		{
			printf("SERVER DISCONNECTED...\nProgram Exiting...\n");
			threadFinished = 1;
			return NULL;
		}
	}//end while
}//end readThread

int main(int argc, char ** argv)
{

	int port = atoi(argv[1]);
	int sockfd,len,ret;
  	struct sockaddr_in saddr;

    	// creating tcp socket
    	sockfd = socket(AF_INET,SOCK_STREAM,0); 
    	saddr.sin_family = AF_INET;
    	saddr.sin_port = htons(port); //from command line
    	saddr.sin_addr.s_addr = inet_addr(argv[2]);//loop back to self (OS loops packet back)
    	len = sizeof(saddr);


    	ret = connect(sockfd,(struct sockaddr *)&saddr,len);
    	if(ret == -1)
	{
        	perror("connect ");
        	exit(1);
    	}
	printf("::CLIENT READY::\n");
	printf("--Connected to Server--\n");

	pthread_t reader;
	pthread_create(&reader, NULL, (void*)*readThread, (void*)&sockfd);

	//CLIENT-SIDE EXAMPLE:
//This is one example (a very explicit one)
//of how to make a packet using the protocol.
//write functions/methods instead of doing it this way.

	//INITIALIZE FIELDS
		char command[1];
		char option[20];
		char size[8];
		char content[256];
		char packet[283]; //the packet size will be the command + option + size + content (which will be the MAX size of a file).

	//NULL OUT FIELDS
		int i = 0;
		command[i] = '\0';
		for(; i < sizeof(option); i++)
			option[i] = '\0';
		for(; i < sizeof(size); i++)
			size[i] = '\0';
		for(; i < sizeof(content); i++)
			content[i] = '\0';
		for(; i < sizeof(packet); i++)
			packet[i] = '\0';

	//COPY DATA INTO FIELDS
		//Having parsed your user input: "/w starkiller45 This is my first sent message!\n"
		//into the correct parts: "w", "starkiller45", "This is my first sent message!\n"
		//and having retreived the sizeof("This is my first sent message!\n")
		//copy the data into the fields.
		
		//COMMAND
		command[0] = 'w'; //the command char parsed out of the user input
		//OPTION		
		snprintf(option,21,"%s","starkiller45"); //the option parsed out of the user input
		//SIZE
		snprintf(size,9,"%d",100000); //the sizeof(the remaining message the user sent)
		//CONTENT
		snprintf(content,257,"%s","This is my first sent message!\n"); //the message

		//use of snprintf(destination, # of bytes to print (copy), type of data, data):
		//NOTE that the # of bytes to print should be 1 more than your buffer
		//because the method adds a null terminating byte to the end (which could overwrite data).
		//Here I'm just copying the data into the fields.

	//COPY FIELDS INTO PACKET
		int x;
		//COMMAND -> packet[0]
		packet[0] = command[0];

		//OPTION -> packet[1 - 20]
		for(x = 1; x < 21; x++)
			packet[x] = option[x-1];
		//SIZE -> packet[21 - 29]
		for(x = 21; x < 29; x++)
			packet[x] = size[x-21];
		//CONTENT -> packet[29 - 284]
		for(x = 29; x < 284; x++)
			packet[x] = content[x-29];
	
	//SHOW PACKET CONTENTS
		//printf("\n");
		//for(x = 0; x < 284; x++)
		//	printf("packet[%d]:%c\n",x,packet[x] == 0 ? '*' : packet[x]);
			//NULLS: '\0' (aka the null terminating byte for strings)
			//are represented here by the '*' character, and the \n character
			//will print out a new line.

	write(sockfd,packet,283);
	//NOTE: printing out the packet as a STRING will result in: wstarkiller45
	//because the null bytes '\0' filling the rest of the OPTION field will terminate the string.

	while(!threadFinished)
	{
		char writes[256];	
		fgets(writes,255,stdin);
		write(sockfd,writes,strlen(writes));
	}

}//end main
