
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

int main (int argc, char *argv[]){


    // char* SERVERIP = "127.0.0.1"; //variables are in capital because for testing I used global variables that i seted up manually
    // size_t PORT = 25555;
    // size_t BUFSIZE =300; //IP adress and port manual set for testing
    
    char* SERVERIP;
    size_t PORT;
    
    if (argc != 3)
    {
       printf("Invalid number of arguments\n");
       printf("please input the ip adress followed by the port after calling the program\n");
       exit(-1);
    }
    else{
        SERVERIP = argv[1];
        PORT = atoi(argv[2]);
        //printf("<%s:%ld>\n", SERVERIP, PORT);
    }




    size_t ansBufsize=20; //define the buffer size for answers 20

    struct sockaddr_in socket_addr;
    if(memset(&socket_addr, 0, sizeof(struct sockaddr_in))==NULL){
        perror("memset error: ");
        exit(EXIT_FAILURE);
    }    
    
    socket_addr.sin_addr.s_addr=inet_addr(SERVERIP); //note: later change these values to the input of the program: argv[]
    socket_addr.sin_family=AF_INET;
    socket_addr.sin_port=htons(PORT);

    int client_fd = socket(AF_INET,SOCK_STREAM,0);  

    if(client_fd==-1){
        perror("socket() :");
        exit(EXIT_FAILURE);
    }

    int connection = connect(client_fd,(struct sockaddr *)& socket_addr, sizeof(struct sockaddr));
    if(connection==-1){
        printf("connection with server failed\n");
        perror("connect(): ");
        exit(-1);
    }
    else{
        printf("connection established with the server\n");
        printf("waiting for server...\n");
    }

    //code to read the question from the server
    char buffer[BUFSIZE];

    readServerMessage(client_fd, buffer);
    printf("%s\n", buffer);
     char *answer;
     getline(&answer, &ansBufsize, stdin);
     writeSereverMessage(client_fd,answer, 2);
    
    //depending on answer continue or stop

    if(strcmp(answer,"Y\n")==0){
            printf("*Note, answers are capital Y, capital N or commands/output just as you would see it in the terminal\n");
            printf("Be careful with indempting & higher or lowercases\n");
            printf("Input your answers and then click <enter>\n");
            for(int i=0;i<5;i++){                
                readServerMessage(client_fd, buffer);
                printf("%s\n", buffer);
                getline(&answer, &ansBufsize, stdin);
                writeSereverMessage(client_fd,answer, 2);
                //printf("%d\n", i);
                readServerMessage(client_fd, buffer);
                printf("%s", buffer);
            }
            //writeSereverMessage(client_fd,answer);
            //printf("all answers were sent\n");

            readServerMessage(client_fd, buffer);
            printf("%s", buffer);
            fflush(stdout);
        }
        else{
            if(strcmp(answer,"q\n")==0){
                printf("\"q\" inputed, the quiz will not continue\n"); 
            }
            else{
                printf("unknown input, quiz will not continue,\n"); 
                printf("next time make sure to input lowercase \"q\" or highercase \"Y\"\n");
            }
        }

    free(answer);
    printf("Quiz has ended, terminating program\n");
    close(client_fd);
    return 0;
}
