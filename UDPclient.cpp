/* Shaina Mae Rosell - 30037911 - CPSC 441 W20 - Assignment 2 
 * A class that creates a UDP client to communicate with a micro-service UDP server
 * 
 * Compile using the compiler for all files
 * "./run.sh"
 */

/* Include files */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "udp.h"    // header file
#include <ctime> // timer socket when receiving from UDP servers

class UDPclient {
    private:
        struct sockaddr_in si_server;
        struct sockaddr *server;
        int s; 
        socklen_t len = sizeof(si_server);
        char buf[MAX_MESSAGE_LENGTH];
        int readBytes;
    public:
    /* 
     * Constructor of this class. Creates the socket specifically to contact
     * a port number of a micro-service UDP server
     * serverPort - port number of the micro-service to communicate with
     */
    UDPclient(uint16_t serverPort) {
        if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
            printf("Could not set up a socket!\n");
            return;
        }
        memset((char *)&si_server, 0, sizeof(si_server));
        si_server.sin_family = AF_INET;
        si_server.sin_port = htons(serverPort);
        server = (struct sockaddr *)&si_server;

        // set a timout (2s) for recvfrom
        struct timeval tv = {2, 0};
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));    

        if (inet_pton(AF_INET, SERVER_IP, &si_server.sin_addr) == 0) {
            printf("inet_pton() failed\n");
            return;
        }

    }
    /*
     * function for sending to and receiving from the micro-service it connects to
     * when using this function, it expects to receive something back from the micro-service
     * and it is placed in the outStr. If time-out happens, the original message is placed
     * as the outStr
     * 
     * msg - message to be modified by the micro-service
     * outStr - out parameter that contains the modified message received from the micro-service
     */
    void sendToMicroService(char* msg, char* outStr) {

        strcpy(buf,msg);
        if (sendto(s, buf, strlen(buf), 0, server, sizeof(si_server)) == -1) {
            printf("sendto failed\n");
            return;
        }

        if ((readBytes = recvfrom(s, buf, MAX_MESSAGE_LENGTH, 0, server, &len)) == -1) {
            fprintf(stderr, "   FAILED: Did not receive anything from micro service! No Data-transformation done \n");
            // return the unmodified msg
            strcpy(outStr,msg);
            return;
        } 

        buf[readBytes] = '\0';  // proper null-termination of string
        strcpy(outStr,buf);    
    }

    void closeClient() {
        close(s);
    }

};




