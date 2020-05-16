/* Shaina Mae Rosell - 30037911 - CPSC 441 W20 - Assignment 2 
 * Micro-Service # 1 Identity
 * Receives a message from the UDP client (Master Server). 
 * 
 * modification of Carey's wordlen-UDPserver
 * 
 * Does nothing to the data but return exactly what was received
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
#include "udp.h"

int identitySocket;

/* Main program */
void * IDudpServer(void *t) {
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;

    socklen_t len = sizeof(si_server);
    char messagein[MAX_MESSAGE_LENGTH];
    char messageout[MAX_MESSAGE_LENGTH];
    int readBytes;

    if ((identitySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("Could not setup a socket!\n");
        return t;
    }

    memset((char *)&si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(IDPORTNUM);
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *)&si_server;
    client = (struct sockaddr *)&si_client;

    if (bind(identitySocket, server, sizeof(si_server)) == -1) {
        printf("Could not bind to port %d!\n", IDPORTNUM);
        return t;
    }
    printf("Identity micro-service now listening on UDP port %d...\n", IDPORTNUM);

    /* big loop, looking for incoming messages from clients */
    for (;;) {
        /* clear out message buffers to be safe */
        bzero(messagein, MAX_MESSAGE_LENGTH);
        bzero(messageout, MAX_MESSAGE_LENGTH);

        /* see what comes in from a client, if anything */
        if ((readBytes = recvfrom(identitySocket, messagein, MAX_MESSAGE_LENGTH, 0, client, &len)) < 0) {
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
        printf("server thinks that word has %d characters...\n", strlen(messagein));
#endif
        /* create the outgoing message (as an ASCII string) */
        sprintf(messageout, "%s", messagein);

#ifdef DEBUG
        printf("Server sending back the message: \"%s\"\n", messageout);
#endif

        /* send the result message back to the client */
        sendto(identitySocket, messageout, strlen(messageout), 0, client, len);
    }

    close(identitySocket);
    return t;
}
