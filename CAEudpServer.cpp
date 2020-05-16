/* Shaina Mae Rosell - 30037911 - CPSC 441 W20 - Assignment 2 
 * Micro-Service # 5 Caesar Cipher
 * Receives a message from the UDP client (Master Server). 
 * Sends back the modified message
 * 
 * modification of Carey's wordlen-UDPserver
 * 
 * Transformation applies a simple Caesar cipher to all
 * alphabetical symbols in a message. Adds a fixed offset of 13 to each
 * letter (with wraparound). ex: A -> N
 * Preserves the case of each letter and characters not in the
 * alphabet remains unchanged
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
#include <string>
#include "udp.h"

using namespace std;
int caesarSocket;

/* Main program */
void * CAEudpServer(void *t) {
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;

    socklen_t len = sizeof(si_server);
    char messagein[MAX_MESSAGE_LENGTH];
    char messageout[MAX_MESSAGE_LENGTH];
    int readBytes;

    if ((caesarSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("Could not setup a socket!\n");
        return t;
    }

    memset((char *)&si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(CAESARPORTNUM);
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *)&si_server;
    client = (struct sockaddr *)&si_client;

    if (bind(caesarSocket, server, sizeof(si_server)) == -1) {
        printf("Could not bind to port %d!\n", CAESARPORTNUM);
        return t;
    }
    printf("Caesar micro-service now listening on UDP port %d...\n", CAESARPORTNUM);

    /* big loop, looking for incoming messages from clients */
    for (;;) {
        /* clear out message buffers to be safe */
        bzero(messagein, MAX_MESSAGE_LENGTH);
        bzero(messageout, MAX_MESSAGE_LENGTH);

        /* see what comes in from a client, if anything */
        if ((readBytes = recvfrom(caesarSocket, messagein, MAX_MESSAGE_LENGTH, 0, client, &len)) < 0) {
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
        // Modification starts here
        string str(messagein,strlen(messagein));
        // 26 letters, index 0 - 25
        string alphabet = "abcdefghijklmnopqrstuvwxyz";

        int i = 0;
        int posInAlph;
        int caesarOffset;
        char c;

        // Iterate through all characters of messagein
        while (messagein[i]) {
            c = messagein[i];
            posInAlph = alphabet.find(tolower(c));
            // characters not in the alphabet are excluded, i.e spaces, special characters, etc. 
            if (posInAlph != -1) {    

                // offset by 13
                caesarOffset = posInAlph + 13;
                // if it exceeds last alphabet, go back to start
                if (caesarOffset > 25) {
                    caesarOffset = (caesarOffset - 25) - 1;
                }

                // case check for the original character
                if (isupper(c) != 0) {
                    messagein[i] = toupper(alphabet.at(caesarOffset));
                } else {
                    messagein[i] = alphabet.at(caesarOffset);
                }            
            }
            i++;
        }

        /* create the outgoing message (as an ASCII string) */
        sprintf(messageout, "%s", messagein);

#ifdef DEBUG
        printf("Server sending back the message: \"%s\"\n", messageout);
#endif

        /* send the result message back to the client */
        sendto(caesarSocket, messageout, strlen(messageout), 0, client, len);
    }

    close(caesarSocket);
    return t;
}
