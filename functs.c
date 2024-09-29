#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>// socket(), and other connections
#include <arpa/inet.h> //inetaddr(), hton() functions
#include <errno.h> //errno
#include "headers.h"

#define BUFSIZE 300
/*
By:
Cesar Rondo Rodriguez
Student num: 22779089
email: cesar.rondorodriguez@ucdconnect.ie
*/
//Function to read a message from another process via sockets
void readServerMessage(int client_fd, char *buffer){
    size_t bytesRead;
    char *bufPtr = buffer;
    for(bytesRead=0;bytesRead<BUFSIZE;){
        ssize_t numRead = read(client_fd, bufPtr, BUFSIZE - bytesRead); //read system call using inputed file descriptor of the used socket
        if(bytesRead<=0){
            if(numRead==0){ //break the loop when there are no more bytes left to read
                break;
            }
            if (numRead == -1) {  //error handling
               if (errno == EINTR)
                  continue;
               else {
                  perror("read(): ");
               }
            }
        }
        bytesRead+=numRead;
        bufPtr+=numRead;
    }
}
//Function to write a message to another process via sockets
void writeSereverMessage(int client_fd, char *initialMessage, int mode ){//mode == 2 for client, mode == 1 for server

    size_t bytesWriten;
    char * buffer = initialMessage;

    ssize_t numWritten=0;

    //printf("%s\n", buffer);
    //printf("%ld\n", sizeof(QuizQ[qIndex]));
    for(bytesWriten=0;bytesWriten<300;){    //300 is the size of the initial message, the questions won't be that big so I use a separate size for the initial message
        if(mode ==1){
            numWritten = write(client_fd, buffer, BUFSIZE - bytesWriten);
        }
        else if(mode == 2){
            numWritten = write(client_fd, buffer + bytesWriten, BUFSIZE - bytesWriten);//write system call using inputed file descriptor of the used socket
        }
        if(numWritten<=0){
            if(numWritten==-1 && errno==EINTR){ //Bytes left to write
                continue;
            }
               else{//write failed
                //printf("Bytes written: %ld\n", bytesWriten);
                perror("write(): ");
                exit(EXIT_FAILURE);
            }
        }
        bytesWriten+=numWritten;
        buffer+=numWritten;
    }
}

void signalHandlerINT(){//Interruot signal handling : Ctrl-C
    printf("\n");
    printf("Ctrl-C pressed\n");
    printf("terminating server\n");
    fflush(stdout);
    exit(0);
}
void signalHandlerTERM(){ //Terminate signal handling
    printf("\n");
    printf("Terminate Signal detected\n");
    printf("terminating server\n");
    fflush(stdout);
    exit(0);
}