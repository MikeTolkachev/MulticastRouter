#ifndef COMMANDSERVER_H
#define COMMANDSERVER_H

#include <stdio.h>      /* standard C i/o facilities */
#include <stdlib.h>     /* needed for atoi() */
#include <unistd.h>     /* defines STDIN_FILENO, system calls,etc */
#include <sys/types.h>  /* system data type definitions */
#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gethostbyname */

class StreamManager;

#define MAXBUF 1024
#define SERVERPORT 8889

class CommandServer
{
public:
    CommandServer();
    ~CommandServer();

    void init(StreamManager *manager);
    void run();

private:
    struct sockaddr_in skaddr;
    socklen_t length;
    int socketDescriptor;
    int packetSize;
    struct sockaddr_in clientaddr;
    socklen_t clientlen;

    StreamManager *mManager;


};

#endif // COMMANDSERVER_H
