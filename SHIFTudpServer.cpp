/* Shaina Mae Rosell - 30037911 - CPSC 441 W20 - Assignment 2 
 * Micro-Service # 6 Shift Middle
 * Receives a message from the UDP client (Master Server). 
 * Sends back the modified message
 * 
 * modification of Carey's wordlen-UDPserver
 * 
 * Word by Word, modifies all characters of a word EXCEPT THE FIRST AND LAST
 * shifts the middle letters to the left by 1 with a wraparound. First and 
 * last character of the word stays in place (this also applies to special characters)
 * ex: Four -> Fuor
 *     Files -> Fleis
 * This "shift middle" Only works on words with len >= 4
 * Words with 3 or lesser, you can't really shift the middle
 * as there is not enough characters to shift (excluding first and last char)
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
int shiftSocket;

/* Main program */
void * SHIFTudpServer(void *t) {
//int main() {
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;
    socklen_t len = sizeof(si_server);
    char messagein[MAX_MESSAGE_LENGTH];
    char messageout[MAX_MESSAGE_LENGTH];
    int readBytes;

    if ((shiftSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        printf("Could not setup a socket!\n");
        return t;
    }

    memset((char *)&si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(SHIFTPORTNUM);
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *)&si_server;
    client = (struct sockaddr *)&si_client;

    if (bind(shiftSocket, server, sizeof(si_server)) == -1) {
        printf("Could not bind to port %d!\n", SHIFTPORTNUM);
        return t;
    }
    printf("Shift micro-service now listening on UDP port %d...\n", SHIFTPORTNUM);

    /* big loop, looking for incoming messages from clients */
    for (;;) {
        /* clear out message buffers to be safe */
        bzero(messagein, MAX_MESSAGE_LENGTH);
        bzero(messageout, MAX_MESSAGE_LENGTH);

        /* see what comes in from a client, if anything */
        if ((readBytes = recvfrom(shiftSocket, messagein, MAX_MESSAGE_LENGTH, 0, client, &len)) < 0) {
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

        // do the shift on the string WORD BY WORD
        // it shift every letter, EXCEPT the first and last, to the the left by one
        string str(messagein,strlen(messagein));
        size_t nonSpacePos = str.find_first_not_of(" ");
        size_t spacesPos;
        size_t wordSz;

        // iterate through all words in the sentence finds the position of each word (where they each start and end)
        // nonSpacePos is the position of the first letter of each word
        // spacesPos is the position of the first space found on the current word being modified
        // to shift the letters, we reverse sections of the letters
        while (nonSpacePos != string::npos) {
            spacesPos = str.find(" ",nonSpacePos);
            if (spacesPos == string::npos) {
                // reached the last word of the sentence so no more space left to find
                // only works on word len >= 4
                if ((wordSz = spacesPos - nonSpacePos) >= 4) {
                    if (wordSz == 4) {
                        reverse((str.begin() + nonSpacePos + 1),(str.begin() + nonSpacePos + 2));
                    }
                    else {
                        reverse((str.begin() + nonSpacePos + 2), (str.end() - 1));
                        reverse((str.begin() + nonSpacePos + 1), (str.end() - 1));
                    }
                }
                break;
            }
            // shifting only works for words with len >= 4
            else if ((wordSz = spacesPos - nonSpacePos) >= 4) {
                if (wordSz == 4) {
                    reverse((str.begin() + nonSpacePos + 1),((str.begin() + spacesPos) - 1));
                }
                else {
                    reverse((str.begin() + nonSpacePos + 2), ((str.begin() + spacesPos) - 1));
                    reverse((str.begin() + nonSpacePos + 1), ((str.begin() + spacesPos) - 1));
                }
            }


            
            nonSpacePos = str.find_first_not_of(" ",spacesPos);
        } 
        
        /* create the outgoing message (as an ASCII string) */
        sprintf(messageout, "%s", str.c_str());

#ifdef DEBUG
        printf("Server sending back the message: \"%s\"\n", messageout);
#endif

        /* send the result message back to the client */
        sendto(shiftSocket, messageout, strlen(messageout), 0, client, len);
    }

    close(shiftSocket);
    return t;
}
