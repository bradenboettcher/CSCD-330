#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <zconf.h>
#include <strings.h>

bool threadFinished = 0;

void *readThread(int *sockfd) {
    int ret, x;
    while (1) {
        char reads[262173];
        bzero(&reads, sizeof(reads));
        ret = read(*sockfd, reads, sizeof(reads));
        if (reads[0] == 'd') {
            printf("SERVER DISCONNECTED...\nProgram Exiting...\n");
            threadFinished = 1;
            return NULL;
        } else if (ret > 0) {
            reads[ret] = '\0';

            if (reads[0] != 'f' && reads[0] != 'g') {}
            for (x = 1; x < 21; x++) {
                printf("%c", reads[x]);
            }
            printf(": ");
            for (x = 29; x < sizeof(reads); x++) {
                printf("%c", reads[x]);
            }
            printf("\n");
            ret = 0;
        }
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
    char content[262144];
    char packet[262173]; //the packet size will be the command + option + size + content (which will be the MAX size of a file).

    char room1[2] = "X";

    while (!threadFinished) {
        bzero(&command, sizeof(command));
        bzero(&option, sizeof(option));
        bzero(&size, sizeof(size));
        bzero(&content, sizeof(content));
        bzero(&packet, sizeof(packet));

        int x;

        char writes[262173];
        bzero(&writes, sizeof(writes));
        fgets(writes, 262173, stdin);

        if (writes[0] == '/') {
            int u;
            //command[0] = writes[1];
            switch (writes[1]) {
                case 'b'    :
                    command[0] = 'b';

                    for (u = 0; u < sizeof(writes) - 3; u++) {
                        content[u] = writes[u + 3];
                    }
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
                    command[0] = 'f';//commands.pFile(clientList,packet);private file
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
                    for (x = 0; x < 21 && (writes[x + 3] != ' ' || writes[x + 3] != '\n'); x++)
                        option[x] = writes[x + 3];
                    break;

                case 'r'    :
                    command[0] = 'r'; //explicitly send message to room
                    for (x = 0; x < sizeof(writes) - 2; x++)
                        content[x] = writes[x + 3];
                    break;

                case 's'    :
                    command[0] = 's';//switch rooms
                    for (x = 0; x < 21; x++)
                        option[x] = writes[x + 3];
                    break;

                case 'w'    :
                    command[0] = 'w';//Whisper

                    for (x = 0; x < 24 && writes[x + 3] != ' '; x++)
                        option[x] = writes[x + 3];


                    puts(option);
                    printf("option: %s\n", option);
                    // leaves at space after name
                    int y = x += 4;

//                    printf("X: %d\n", x);
//                    printf("Y: %d\n", y);
//



                    for (; x < sizeof(content) + y; x++)/////// puts message into conent///
                    {
                        content[(x - y)] = writes[x];
                    }


                    break;


/////////////////////////////////////////////Invalid Command - should be on server
                default        :
                    printf("%c%c Is not a valid command. Type /c for list of current commands\n", writes[0], writes[1]);
                    command[0] = writes[1];
                    break;
            }

//            for (u = 0; u < 20; u++) {
//                printf("%c\n", option[u]);
//            }
        }
            /////////////////////////////////////No Command Given//////////////////////
        else {
            int u;
            puts("else");
            command[0] = 'r';
            puts("filled command");

            for (u = 0; u < sizeof(writes); u++) {
                content[u] = writes[u];
            }


//            printf("command: %s\n", command);
//            printf("option: %s\n", option);
//            printf("size: %s\n", size);
//            printf("Content: %s\n", content);
//            puts("filled content");

        }

/////////////////////////////////////////BUILDING PACKET///////////////////////////////

//        printf("command: %s\n", command);
//        printf("option: %s\n", option);
//        printf("size: %s\n", size);
//        printf("Content: %s\n", content);
//        puts("filled content");

        if (command[0] != 'z') {
            //content 262144
            packet[0] = command[0];
            for (x = 1; x < 21; x++) {
                if (option[x - 1] == '\n')
                    packet[x] = '\0';

                else
                    packet[x] = option[x - 1];
            }
            //SIZE -> packet[21 - 29]
            for (x = 21; x < 29; x++) {
                packet[x] = size[x - 21];
            }
            //CONTENT -> packet[29 - 284]
            for (x = 29; x < 262144; x++) {
                if (content[x - 29] == '\n')
                    packet[x] = '\0';
                else
                    packet[x] = content[x - 29];
            }
            puts("PACKET");
            //puts(packet);
            for (x = 0; x < 262173; x++) {
                printf("%c", packet[x]);
            }
            printf("packetlength: %d\n", (int) strlen(packet));
            printf("messagelength: %d\n", (int) strlen(packet) - 30);
        }//end If
//////////////////////////////////////PACKET BUILT//////////////////////////////////////
        write(sockfd, packet, sizeof(packet));
        bzero(&command, sizeof(command) + 1);
        bzero(&option, sizeof(option) + 1);
        bzero(&size, sizeof(size) + 1);
        bzero(&content, sizeof(content) + 1);


    }

}//end main
