
// Shaina Mae Rosell - 30037911 - CPSC 441 W20 - Assignment 2 

#define MAX_MESSAGE_LENGTH 100  // the same length as buffer size in microservices

#define SERVER_IP "127.0.0.1" // loop back interface, client and server in the same device
/*  Device IP - specify the device ip address (ipv4) in which the server is running on
 allows TCPclients in different devices to communicate to a TCPserver */
//#define SERVER_IP "<device-ip>"

#define MYPORTNUM 30037        // Main port number

#define IDPORTNUM 1111        // Identity micro service port number
#define REVPORTNUM 2222
#define UPPERPORTNUM 3333
#define LOWERPORTNUM 4444
#define CAESARPORTNUM 5555
#define SHIFTPORTNUM 6666

// transformation options
#define IDENTITY "1"
#define REVERSE "2"
#define UPPER "3"
#define LOWER "4"
#define CAESAR "5"
#define SHIFT "6"

//#define DEBUG 1

/*  These sockets are in their respective micro-services files
Followed by void functions that starts each micro-service as a UDP server, 
used in startMicroServices()  */

extern int identitySocket;    // IDudpServer.cpp
void * IDudpServer(void *t);

extern int reverseSocket;     // REVudpServer.cpp
void * REVudpServer(void *t);

extern int upperSocket;       // UPPudpServer.cpp
void * UPPudpServer(void *t);

extern int lowerSocket;       // LOWudpServer.cpp
void * LOWudpServer(void *t);

extern int caesarSocket;      // CAEudpServer.cpp
void * CAEudpServer(void *t);

extern int shiftSocket;       // SHIFTudpServer.cpp
void * SHIFTudpServer(void *t);



/*  Startup micro-services when master server is started
 Uses pthreads to run the micro-services in their own UDP-based servers.
 These functions are in TCPserver.cpp */
void startMicroServices();
void closeMicroServices();
void closeClients();
