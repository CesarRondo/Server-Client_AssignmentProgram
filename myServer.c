
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h> //socket()
#include <netdb.h>
#include <arpa/inet.h> //inetaddr() inet
#include <errno.h> //error codes to know when we have finished reading or sending a message
#include <time.h>
#include <signal.h>
#include "quizDB.h"
#include "headers.h"

#define BUFSIZE 300

/*
By:
Cesar Rondo Rodriguez
email: c.rondoro12@gmail.com
*/

int main (int argc, char *argv[]){

    // char* SERVERIP = "127.0.0.1"; //variables are in capital because for testing I used global variables that i seted up manually
    // size_t PORT = 25555;        //IP adress and port manual set for testing
    // size_t BUFSIZE =300;
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


    //seting up signal handlers
    if(signal(SIGINT,signalHandlerINT)==SIG_ERR){
        printf("signal handler could not be set up\n");
    }
    if(signal(SIGTERM,signalHandlerTERM)==SIG_ERR){
        printf("signal handler could not be set up\n");
    }



    struct sockaddr_in socket_addr;
    if(memset(&socket_addr, 0, sizeof(struct sockaddr_in))==NULL){ //memset calls perror
        perror("memset error:");
        exit(EXIT_FAILURE);
    }
    
    socket_addr.sin_addr.s_addr=inet_addr(SERVERIP); //note: later change these values to the input of the program: argv[]
    socket_addr.sin_family=AF_INET;
    socket_addr.sin_port=htons(PORT);

    int socket_fd = socket(AF_INET,SOCK_STREAM,0); //create a file descriptor for the new socket
    if(socket_fd==-1){
        perror("socket() :");
        exit(EXIT_FAILURE);
    }
    /*
    Code for sockopt if needed
    */
    int errorCheck;

    errorCheck=bind(socket_fd,(struct sockaddr*)&socket_addr, sizeof(struct sockaddr));//bind file descriptor and socket struct created
    if(errorCheck==-1){
        perror("bind() :");
        exit(EXIT_FAILURE);

    }

    errorCheck=listen(socket_fd,3);    //server queues 3 clients at most
                            //socket_fd stays listening for more clients
    if(errorCheck==-1){
        perror("listen() :");
        exit(EXIT_FAILURE);

    }
    printf("listening on <%s:%ld>\n", SERVERIP, PORT);
    printf("<Press ctrl-C to terminate>\n");
    while(1){
        
        struct sockaddr_storage claddr;//create socket for client that is being accepted
        int clientAddrSize = sizeof(struct sockaddr_storage);
        int client_socket_fd= accept(socket_fd, (struct sockaddr*)&claddr, &clientAddrSize);
        if(client_socket_fd==-1){
            printf("error in accepting client\n");
            perror("accept(): ");
            exit(EXIT_FAILURE);
        }
        printf("Client connected\n");
        
        //code to send the initial message to the client
        char initialMessage[300]="";
        strcat(initialMessage,"Welcome to the Unix Programming Quiz!\n");
        strcat(initialMessage,"The quiz comprises five questions posed to you one after the other.\n");
        strcat(initialMessage,"You have only one atempt to answer a question.\n");
        strcat(initialMessage,"Your final score will be sent to you after the conclusion of the quiz.\n");
        strcat(initialMessage,"to start the quiz press Y and <enter>.\n");
        strcat(initialMessage,"To quit the quiz press q and <enter>.\n");

        writeSereverMessage(client_socket_fd,initialMessage, 1);


        //read and show client answer
        char answer[BUFSIZE];
        readServerMessage(client_socket_fd, answer);
        //printf("user answer is: %s\n", answer);

        if(strcmp(answer,"Y\n")==0){
            int finalResult=0;
            char * buffer;
            int prevQ[5];
            srand(time(NULL));
            for(int i=0;i<5;i++){                
                printf("%d\n", i);
                int qIndex=(rand()%42);//Randomly generate the question to be asked to the client
                
                for(int j=0;j<i;j++){     //nested loop to check that the new random question is not a repeat of previous ones
                    if(qIndex==prevQ[j]){
                        qIndex=(rand()%42);
                        j=0;
                    }
                }
                prevQ[i]=qIndex;

                buffer=QuizQ[qIndex];
                writeSereverMessage(client_socket_fd,buffer, 1);
                readServerMessage(client_socket_fd, answer);
                printf("%s\n", answer);

                buffer = QuizA[qIndex];
                char rightAnswer[BUFSIZE];
                strcpy(rightAnswer, "");
                strcat(rightAnswer,buffer);
                strcat(rightAnswer,"\n");
                if(strcmp(answer, rightAnswer)==0){
                   finalResult++;; 
                   buffer = "Right answer\n";
                   writeSereverMessage(client_socket_fd,buffer, 1);
                }
                else{
                   strcpy(rightAnswer,"Wrong answer. Right answer is: <");
                   strcat(rightAnswer,QuizA[qIndex]);
                   strcat(rightAnswer,">\n");
                   buffer=rightAnswer;
                   writeSereverMessage(client_socket_fd,buffer, 1); 
                }
            }
            printf("Quiz has finished\n");
            printf("Final score: %d\n", finalResult);
            fflush(stdout);
            char finalResultString[20];//make cahracter array because sprintf doesnt support char pointers
            sprintf(finalResultString,"Final Score: %d\n",finalResult);
            printf("sprintf executed\n");
            buffer=finalResultString;
            writeSereverMessage(client_socket_fd,buffer, 1);
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

        close(client_socket_fd);
        printf("Quiz terminated, closing connection\n");
        printf("listening on <%s:%ld>\n", SERVERIP, PORT);
        printf("<Press ctrl-C to terminate>\n");

    }

    close(socket_fd);
    return 0;
}
