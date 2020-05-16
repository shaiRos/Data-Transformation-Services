/* Shaina Mae Rosell - 30037911 - CPSC 441 W20 - Assignment 2 
 * Micro-Service # 2 Reverse 
 * Receives a message from the UDP client (Master Server). 
 * Sends back the modified message
 * 
 * modification of Carey's wordlen-UDPserver
 * 
 * Modified Word by Word, not by Sentence
 * Reverses the characters of each word(s) and returns the
 * result back to the UDP client
 * 
 * Compile using the compiler for all files
 * "./run.sh"
 */

/* Include files */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "udp.h"

#include <bits/stdc++.h> // for reverse

using namespace std;
int reverseSocket;

/* Main program */
void * REVudpServer(void *t) {
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;
    socklen_t len = sizeof(si_server);
    char messagein[MAX_MESSAGE_LENGTH];
    char messageout[MAX_MESSAGE_LENGTH];
    int readBytes;

    if ((reverseSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("Could not setup a socket!\n");
        return t;
    }

    memset((char *)&si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(REVPORTNUM);
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *)&si_server;
    client = (struct sockaddr *)&si_client;

    if (bind(reverseSocket, server, sizeof(si_server)) == -1) {
        printf("Could not bind to port %d!\n", REVPORTNUM);
        return t;
    }
    printf("Reverse micro-service now listening on UDP port %d...\n", REVPORTNUM);

    /* big loop, looking for incoming messages from clients */
    for (;;) {
        /* clear out message buffers to be safe */
        bzero(messagein, MAX_MESSAGE_LENGTH);
        bzero(messageout, MAX_MESSAGE_LENGTH);

        /* see what comes in from a client, if anything */
        if ((readBytes = recvfrom(reverseSocket, messagein, MAX_MESSAGE_LENGTH, 0, client, &len)) < 0) {
            printf("Read error!\n");
            return t;
        }
#ifdef DEBUG
        else
            printf("Server received %d bytes\n", readBytes);
#endif

        printf("  server received \"%s\" from IP %s port %d\n",
               messagein, inet_ntoa(si_client.sin_addr), ntohs(si_client.sin_port));

#ifdef DEBUG
        printf("server thinks that word has %d characters...\n", (int) strlen(messagein));
#endif

        // reverse the string word by word
        string str(messagein,strlen(messagein));
        size_t nonSpacePos = str.find_first_not_of(" ");
        size_t spacesPos;

        // iterate through all words in the sentence finds the position of each word (where they each start and end)
        // then reverse each words
        while (nonSpacePos != string::npos) {
            
            spacesPos = str.find(" ",nonSpacePos); 
            if (spacesPos == string::npos) {
                reverse(str.begin() + nonSpacePos,str.end());
                break;
            }
            reverse(str.begin() + nonSpacePos,str.begin() + spacesPos);
            nonSpacePos = str.find_first_not_of(" ",spacesPos);
        } 
        
        /* create the outgoing message (as an ASCII string) */
        sprintf(messageout, "%s", str.c_str());

#ifdef DEBUG
        printf("Server sending back the message: \"%s\"\n", messageout);
#endif

        /* send the result message back to the client */
        sendto(reverseSocket, messageout, strlen(messageout), 0, client, len);
    }

    close(reverseSocket);
    return t;
}
