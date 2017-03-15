#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <zconf.h>
#include <strings.h>

bool threadFinished = 0;

void *readThread(int *sockfd) {
    int ret;
    while (1) {
        char reads[262173];
        ret = read(*sockfd, reads, sizeof(reads));
        if (reads[0] == 'd') {
            printf("SERVER DISCONNECTED...\nProgram Exiting...\n");
            threadFinished = 1;
            return NULL;
        } else if (ret > 0) {
            reads[ret] = '\0';
            printf("SERVER: %s", reads);
        }
    }//end while
}//end readThread

int main(int argc, char **argv) {

    int port = atoi(argv[1]);
    int sockfd, len, ret;
    struct sockaddr_in saddr;

    // creating tcp socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port); //from command line
    saddr.sin_addr.s_addr = inet_addr(argv[2]);//loop back to self (OS loops packet back)
    len = sizeof(saddr);


    ret = connect(sockfd, (struct sockaddr *) &saddr, len);
    if (ret == -1) {
        perror("connect ");
        exit(1);
    }
    printf("::CLIENT READY::\n");
    printf("--Connected to Server--\n");

    pthread_t reader;
    pthread_create(&reader, NULL, (void *) *readThread, (void *) &sockfd);

    //CLIENT-SIDE EXAMPLE:
//This is one example (a very explicit one)
//of how to make a packet using the protocol.
//write functions/methods instead of doing it this way.

    //INITIALIZE FIELDS
    char command[1];
    char option[20];
    char size[8];
    char content[2262155];
    char packet[262173]; //the packet size will be the command + option + size + content (which will be the MAX size of a file).

    //NULL OUT FIELDS
//    int i = 0;
//    command[i] = '\0';
//    for (; i < sizeof(option); i++)
//        option[i] = '\0';
//    for (; i < sizeof(size); i++)
//        size[i] = '\0';
//    for (; i < sizeof(content); i++)
//        content[i] = '\0';
//    for (; i < sizeof(packet); i++)
//        packet[i] = '\0';

//    //COMMAND
//    command[0] = 'w'; //the command char parsed out of the user input
//    //OPTION
//    snprintf(option, 21, "%s", "starkiller45"); //the option parsed out of the user input
//    //SIZE
//    snprintf(size, 9, "%d", 100000); //the sizeof(the remaining message the user sent)
//    //CONTENT
//    snprintf(content, 257, "%s", "This is my first sent message!\n"); //the message


//    int x;
//    //COMMAND -> packet[0]
//    packet[0] = command[0];
//
//    //OPTION -> packet[1 - 20]
//    for (x = 1; x < 21; x++)
//        packet[x] = option[x - 1];
//    //SIZE -> packet[21 - 29]
//    for (x = 21; x < 29; x++)
//        packet[x] = size[x - 21];
//    //CONTENT -> packet[29 - 284]
//    for (x = 29; x < 284; x++)
//        packet[x] = content[x - 29];

    //SHOW PACKET CONTENTS
    //printf("\n");
    //for(x = 0; x < 284; x++)
    //	printf("packet[%d]:%c\n",x,packet[x] == 0 ? '*' : packet[x]);
    //NULLS: '\0' (aka the null terminating byte for strings)
    //are represented here by the '*' character, and the \n character
    //will print out a new line.

    //write(sockfd, packet, 283);
    //NOTE: printing out the packet as a STRING will result in: wstarkiller45
    //because the null bytes '\0' filling the rest of the OPTION field will terminate the string.

    while (!threadFinished) {
//        bzero(&command, sizeof(command) + 1);
//        bzero(&option, sizeof(option) + 1);
//        bzero(&size, sizeof(size) + 1);
//        bzero(&content, sizeof(content) + 1);
//        bzero(&packet, sizeof(packet));

        int x;

        int i = 0;
        command[i] = '\0';
        for (; i <= sizeof(option); i++) {
            option[i] = '\0';
        }
        for (; i <= sizeof(size); i++) {
            size[i] = '\0';
        }
//        for (; i < sizeof(content); i++)
//            content[i] = '\0';
        for (; i <= sizeof(packet); i++) {
            packet[i] = '\0';
        }


        char writes[262155];
        fgets(writes, 262155, stdin);


        char *p = packet;

        if (writes[0] == '/') {
            switch (writes[1]) {
                case 'b'    :
                    command[0] = 'b';
                    break;

                case 'c'    :
                    command[0] = 'c';//command list//commands.commandList(i);break;
                    break;

                case 'd'    :
                    command[0] = 'd';//commands.disconnect(i);
                    break;

                case 'e'    :
                    command[0] = 'e';//commands.exitRoom(i);
                    break;

                case 'f'    :
                    command[0] = 'f';//commands.pFile(clinetList,packet);private file
                    break;

                case 'g'   :
                    command[0] = 'g';//group file
                    break;
                case 'h'    :
                    command[0] = 'h';//who's in my room?
                    break;

                case 'l'    :
                    command[0] = 'l';//list all users in all rooms
                    break;

                case 'n'    :
                    command[0] = 'n';//name registration
                    break;

                case 'r'    :
                    command[0] = 'r';
                    for (x = 1; x < sizeof(writes) - 2; x++)
                        content[x] = writes[x + 2];//explicitly send message to room
                    break;

                case 's'    :
                    command[0] = 's';//switch rooms
                    break;

                case 'w'    :
                    command[0] = 'w';//Whisper
                    break;

                default        :
                    break;
            }
        } else {
            int u;
            puts("else");
            command[0] = 'r';
            puts("filled command");

            for (u = 0; u < sizeof(writes); u++) {
                // printf("%d\n", u);
                content[u] = writes[u];
            }


            printf("command: %s\n", command);
            printf("option: %s\n", option);
            printf("size: %s\n", size);
            printf("Content: %s\n", content);
            puts("filled content");
            //puts(packet);
        }

        puts("end else");

        //content 262144
        packet[0] = command[0];
        for (x = 1; x < 21; x++) {
            if (option[x - 1] == '\0')
                packet[x] = '\0';
            else
                packet[x] = option[x - 1];
        }
        //SIZE -> packet[21 - 29]
        for (x = 21; x < 29; x++) {
            if (size[x - 21] == '\0')
                packet[x] = '\0';
            else
                packet[x] = size[x - 21];
        }
        //CONTENT -> packet[29 - 284]
        for (x = 29; x < 262115; x++) {

            packet[x] = content[x - 29];
        }
        puts("PACKET");
        for (x = 0; x < 1000; x++) {
            printf("%c", packet[x]);
        }
        // write(sockfd, packet, strlen(packet));
    }

}//end main
