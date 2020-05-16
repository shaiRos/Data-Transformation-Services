/*
 *  Shaina Mae Rosell - 30037911 - CPSC 441 W20 - Assignment 2 Data Transformation Services
 *  TCP-based client that interacts with a TCP-server master server
 * 
 *  Modified Carey's wordlen-TCPclient file 
 * 
 *  A client connects to the master server.
 *  Initially receives a word/sentence from the client then enters a loop 
 *  of interaction with the master server. Within the loop, client can specify what 
 *  data transformations, should be done on the original sentence source data, and in
 *  what order. These requests may involve one or more data transformation, to be
 *  performed in the order specified.
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
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include "udp.h"

using namespace std;

/* Some generic error handling stuff */
void perror(const char *s);

/* Manifest constants used by client program */
#define MAX_HOSTNAME_LENGTH 64

/* Menu selections */
#define ALLDONE 0
#define DOTRANSFORM 1


/* Prompt the user to enter a word */
void printmenu() {
    printf("\n");
    printf("===============================================\n");
    printf("Please choose from the following selections:\n");
    printf("  1 - Transform words\n");
    printf("  0 - Exit program\n");
    printf("Your desired menu selection? ");
}

void printTransformations() {
    printf("  1 - Identity\n");
    printf("  2 - Reverse\n");
    printf("  3 - Upper\n");
    printf("  4 - Lower\n");
    printf("  5 - Caesar\n");
    printf("  6 - Shift Middle\n");
    printf("Specify transformations: ");
}



/* Main program of client */
int main() {
    int clientSock, sockfd2;
    char c;
    struct sockaddr_in server;
    struct hostent *hp;
    char hostname[MAX_HOSTNAME_LENGTH];
    char message[MAX_MESSAGE_LENGTH];
    char messageback[MAX_MESSAGE_LENGTH];
    int choice, len, bytes;

    /* Initialization of server sockaddr data structure */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(MYPORTNUM);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /* hard code the IP address so you don't need hostname resolver */
    //server.sin_addr.s_addr = inet_addr("136.159.5.25");
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    /* create the client socket for its transport-level end point */
    if ((clientSock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "dataTransformation: socket() call failed!\n");
        exit(1);
    }

    /* connect the socket to the server's address using TCP */
    if (connect(clientSock, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "dataTransformation: connect() call failed!\n");
        perror(NULL);
        exit(1);
    }

    /* Print welcome banner */
    printf("Welcome! To my Data Transformation Services client!!\n");
    len = 0;
    while (len == 0) {
        printf("Write a word/sentence (max %d characters):\n",MAX_MESSAGE_LENGTH);
	    while( (c = getchar()) != '\n' ) {
	    message[len] = c;
	    len++;
	    }
    }


	/* cuts the sentence/word if it is more than the MAX_MESSAGE_LENGTH.
       send it to the server via the socket */
    if (len >= MAX_MESSAGE_LENGTH) {
         /* make sure the message is null-terminated in C */
        message[MAX_MESSAGE_LENGTH] = '\0';
        send(clientSock, message, MAX_MESSAGE_LENGTH, 0);
    } else {
	    /* make sure the message is null-terminated in C */
	    message[len] = '\0'; 
        send(clientSock, message, strlen(message), 0);       
    }

	

    printmenu();
    scanf("%d", &choice);

    /* main loop: read a word, send to server, and print answer received */
    while (choice != ALLDONE) {
        if (choice == DOTRANSFORM) {
            /* get rid of newline after the (integer) menu choice given */
            c = getchar();

            /* prompt user for the input */
            printTransformations();
            len = 0;
            while ((c = getchar()) != '\n') {
                message[len] = c;
                len++;
            }

	        /* send it to the server via the socket */
            if (len >= MAX_MESSAGE_LENGTH) {
                 /* make sure the message is null-terminated in C */
                message[MAX_MESSAGE_LENGTH] = '\0';
                send(clientSock, message, MAX_MESSAGE_LENGTH, 0);
            } else {
	            /* make sure the message is null-terminated in C */
	            message[len] = '\0'; 
                send(clientSock, message, strlen(message), 0);       
            }

            /* see what the server sends back */
            if ((bytes = recv(clientSock, messageback, MAX_MESSAGE_LENGTH, 0)) > 0) {
                // make sure the message is null-terminated in C 
                messageback[bytes] = '\0';
                printf("\nAnswer received from server: ");
                printf("%s", messageback);
                printf("\nBytes received: %d\n", bytes);
                bzero(messageback,MAX_MESSAGE_LENGTH);

            } else {
                // an error condition if the server dies unexpectedly 
                printf("Server closed or has failed!\n");
                close(clientSock);
                exit(1);
            } 


        } else
            printf("Invalid menu selection. Please try again.\n");

        printmenu();
        scanf("%d", &choice);
    }

    /* Program all done, so clean up and exit the client */
    close(clientSock);
    exit(0);
}
