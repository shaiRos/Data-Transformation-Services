/*
 *  Shaina Mae Rosell - 30037911 - CPSC 441 W20 - Assignment 2 Data Transformation Services
 *  TCP-based server "master server" that interacts with a TCP-client
 *  
 *  Modified Carey's wordlen-TCPserver file  
 * 
 *  A client connects to this master server.
 *  Once this server is started, it creates the UDPclients
 *  needed for communicating with the micro-services and also starts up
 *  each micro-service in it's own threads. 
 * 
 *  Data transformation options include:
 *  1 Identity
 *  2 Reverse
 *  3 Upper
 *  4 Lower
 *  5 Caesar
 *  6 Shift Middle
 *  
 * Compile using the compiler for all files
 * "./run.sh"
 */

/* Include files for C socket programming and stuff */
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>

#include <ctype.h>

#include "udp.h"
#include "UDPclient.cpp"

using namespace std;

/* Global variable */
int childsockfd;
char messagein[MAX_MESSAGE_LENGTH];
char messageout[MAX_MESSAGE_LENGTH];
char transformedMsg[MAX_MESSAGE_LENGTH];
char savedmsg[MAX_MESSAGE_LENGTH];

// UDPclient objects are created. and the sockets are created using the constructor
// UDP clients are created to communicate with the micro-services (in UDP servers)
// micro-services are specified by their port number
UDPclient identityClient(IDPORTNUM);
UDPclient reverseClient(REVPORTNUM);
UDPclient upperClient(UPPERPORTNUM);
UDPclient lowerClient(LOWERPORTNUM);
UDPclient caesarClient(CAESARPORTNUM);
UDPclient shiftClient(SHIFTPORTNUM);

/* This is a signal handler to do graceful exit if needed */
void catcher(int sig) {
    close(childsockfd);
    printf("\nCaught signal %d from catcher\n",sig);
    printf("Closing UDP-clients\n");
    closeClients();    
    printf("Closing Micro-Services\n");
    closeMicroServices();
    printf("closing TCP master server\n");
    // exits other threads too
    exit(0);
}


/* Main program for server */
int main() {
    struct sockaddr_in server;
    static struct sigaction act;

    int parentsockfd;
    int i, j;
    int pid;
    char c;

    /* Set up a signal handler to catch some weird termination conditions. */
    // modified this a bit to catch ctrl+c 
    // https://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event
    act.sa_handler = catcher;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    //sigfillset(&(act.sa_mask));
    //sigaction(SIGPIPE, &act, NULL);

    /* Initialize server sockaddr structure */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(MYPORTNUM);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /* set up the transport-level end point to use TCP */
    if ((parentsockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "dataTransformation-TCPserver: socket() call failed!\n");
        exit(1);
    }

    /* bind a specific address and port to the end point */
    if (bind(parentsockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "dataTransformation-TCPserver: bind() call failed!\n");
        exit(1);
    }

    /* start listening for incoming connections from clients */
    if (listen(parentsockfd, 5) == -1) {
        fprintf(stderr, "dataTransformation-TCPserver: listen() call failed!\n");
        exit(1);
    }

    /* initialize message strings just to be safe (null-terminated) */
    bzero(messagein, MAX_MESSAGE_LENGTH);
    bzero(messageout, MAX_MESSAGE_LENGTH);

    fprintf(stderr, "Welcome! To my Data Transformation master server!!\n");
    fprintf(stderr, "server listening on TCP port %d...\n\n", MYPORTNUM);


    startMicroServices();
    /* Main loop: server loops forever listening for requests */
    for (;;) {
        /* accept a connection */
        if ((childsockfd = accept(parentsockfd, NULL, NULL)) == -1) {
            fprintf(stderr, "dataTransformation-TCPserver: accept() call failed!\n");
            exit(1);
        }

        /* try to create a child process to deal with this new client */
        pid = fork();

        /* use process id (pid) returned by fork to decide what to do next */
        if (pid < 0) {
            fprintf(stderr, "dataTransformation-TCPserver: fork() call failed!\n");
            exit(1);
        } else if (pid == 0) {
            /* the child process is the one doing the "then" part */

            /* don't need the parent listener socket that was inherited */
            close(parentsockfd);

            

            /* obtain the message from this client */
            int stop = 0;
            int recvBytes;
            int sentBytes;
            while (recvBytes = recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0) > 0) {
				// saving source message only done on the first recv
                if (strlen(savedmsg) == 0) {
                    strcpy(savedmsg, messagein);
                    /* clear out message strings again to be safe */ 

                    bzero(messagein, MAX_MESSAGE_LENGTH);                  
                } else {  
                    // Only receives numbered instructions from the client
                    // on how to modified the saved message
                    printf("\nMaster Server saved message: %s\n\n", savedmsg);


                    // the transformation instructions from client in a string
                    string transform(messagein,strlen(messagein));

                    strcpy(messageout,savedmsg);
                    // iterate through transformation instructions from client
                    while (transform.length() > 0) {

                        if (!transform.rfind(IDENTITY,0)) {
                            printf("1 IDENTITY Transformation:\n");
                            identityClient.sendToMicroService(messageout,transformedMsg);
                            printf("  %s\n",transformedMsg);
                            strcpy(messageout,transformedMsg); 
                        } 
                        else if (!transform.rfind(REVERSE,0)) {
                            printf("2 REVERSE Transformation: \n");
                            reverseClient.sendToMicroService(messageout,transformedMsg);
                            printf("  %s\n",transformedMsg);
                            strcpy(messageout,transformedMsg);                             
                        } 
                        else if (!transform.rfind(UPPER,0)) {
                            printf("3 UPPER Case Transformation:\n");
                            upperClient.sendToMicroService(messageout,transformedMsg);
                            printf("  %s\n",transformedMsg);
                            strcpy(messageout,transformedMsg);  
                        } 
                        else if (!transform.rfind(LOWER,0)) {
                            printf("4 LOWER Case Transformation:\n");
                            lowerClient.sendToMicroService(messageout,transformedMsg);
                            printf("  %s\n",transformedMsg);
                            strcpy(messageout,transformedMsg);  
                        } 
                        else if (!transform.rfind(CAESAR,0)) {
                            printf("5 CAESAR CIPHER Transformation:\n");
                            caesarClient.sendToMicroService(messageout,transformedMsg);
                            printf("  %s\n",transformedMsg);
                            strcpy(messageout,transformedMsg);  
                        }
                        else if (!transform.rfind(SHIFT,0)) {
                            printf("6 SHIFT Case Transformation:\n");
                            shiftClient.sendToMicroService(messageout,transformedMsg);
                            printf("  %s\n",transformedMsg);
                            strcpy(messageout,transformedMsg);  
                        }                        
                        // take out front
                        transform.erase(0,1);

                    }
                    /* create the outgoing message (as an ASCII string) */
                    sprintf(messageout, "%s", messageout);
                    /* send the result message back to the client */
                    sentBytes = send(childsockfd, messageout, strlen(messageout), 0);
                    printf("Sent to client %d Bytes: %s\n", sentBytes, messageout);
                    printf("===============================================\n");

                    /* clear out message strings again to be safe */ 
                    bzero(messagein, MAX_MESSAGE_LENGTH);
                    bzero(messageout, MAX_MESSAGE_LENGTH);
                }
                
            }

            /* when client is no longer sending information to us, */
            /* the socket can be closed and the child process terminated */
            close(childsockfd);
            printf("A client closed\n");
            exit(0);
        } /* end of then part for child */
        else {
            /* the parent process is the one doing the "else" part */
            fprintf(stderr, "\nCreated child process %d to handle that client\n", pid);
            fprintf(stderr, "Parent going back to job of listening...\n\n");

            /* parent doesn't need the childsockfd */
            close(childsockfd);
        }
    }
}


/*  Startup micro-services when master server is started
 Uses pthreads to run the micro-services in their own UDP-based servers */
void startMicroServices() {
    // can specify up to which microservices are available
    int microServices = 6;
    pthread_t threads[microServices];
    int val;
    int i;
    for( i = 1; i <= microServices; i++ ) {

        switch(i) {

            case 1: val = pthread_create(&threads[i], NULL, IDudpServer, (void *) (intptr_t) i ); break;
            case 2: val = pthread_create(&threads[i], NULL, REVudpServer, (void *) (intptr_t) i ); break;
            case 3: val = pthread_create(&threads[i], NULL, UPPudpServer, (void *) (intptr_t) i ); break;
            case 4: val = pthread_create(&threads[i], NULL, LOWudpServer, (void *) (intptr_t) i ); break;
            case 5: val = pthread_create(&threads[i], NULL, CAEudpServer, (void *) (intptr_t) i ); break;
            case 6: val = pthread_create(&threads[i], NULL, SHIFTudpServer, (void *) (intptr_t) i ); break;
            
        }
        if (val) {
            printf("Error:unable to start microservice\n");
        }
    }
}

// these sockets are in udp.h
void closeMicroServices() {
    close(identitySocket);
    close(reverseSocket);
    close(upperSocket);
    close(lowerSocket);
    close(caesarSocket);
    close(shiftSocket);
}

// closes the client by calling the object's
// function for closing client
void closeClients() {
    identityClient.closeClient();
    reverseClient.closeClient();
    upperClient.closeClient();
    lowerClient.closeClient();
    caesarClient.closeClient();
    shiftClient.closeClient();
}


