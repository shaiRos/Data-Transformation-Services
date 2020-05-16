/* Shaina Mae Rosell - 30037911 - CPSC 441 W20 - Assignment 2 
 * Micro-Service # 4 Lower
 * Receives a message from the UDP client (Master Server). 
 * Sends back the modified message
 * 
 * modification of Carey's wordlen-UDPserver
 * 
 * Transforms all alphabetical letters to Lower case
 * 
 * Compile using the compiler for all files
 * "./run.sh"
 */

/* Include files */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>      // tolower
#include "udp.h"

int lowerSocket;

/* Main program */
void * LOWudpServer(void *t) {
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;
    socklen_t len = sizeof(si_server);
    char messagein[MAX_MESSAGE_LENGTH];
    char messageout[MAX_MESSAGE_LENGTH];
    int readBytes;

    if ((lowerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("Could not setup a socket!\n");
        return t;
    }

    memset((char *)&si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(LOWERPORTNUM);
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *)&si_server;
    client = (struct sockaddr *)&si_client;

    if (bind(lowerSocket, server, sizeof(si_server)) == -1) {
        printf("Could not bind to port %d!\n", LOWERPORTNUM);
        return t;
    }
    printf("Lower micro-service now listening on UDP port %d...\n", LOWERPORTNUM);

    /* big loop, looking for incoming messages from clients */
    for (;;) {
        /* clear out message buffers to be safe */
        bzero(messagein, MAX_MESSAGE_LENGTH);
        bzero(messageout, MAX_MESSAGE_LENGTH);

        /* see what comes in from a client, if anything */
        if ((readBytes = recvfrom(lowerSocket, messagein, MAX_MESSAGE_LENGTH, 0, client, &len)) < 0) {
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

        // convert to lower case here
        int i = 0;
        char c;
        while (messagein[i]) {
            c = messagein[i];
            messagein[i] = tolower(c);
            i++;
        }

        /* create the outgoing message (as an ASCII string) */
        sprintf(messageout, "%s", messagein);

#ifdef DEBUG
        printf("Server sending back the message: \"%s\"\n", messageout);
#endif

        /* send the result message back to the client */
        sendto(lowerSocket, messageout, strlen(messageout), 0, client, len);
    }

    close(lowerSocket);
    return t;
}
