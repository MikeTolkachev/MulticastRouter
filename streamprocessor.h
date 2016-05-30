#ifndef STREAMPROCESSOR_H
#define STREAMPROCESSOR_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/time.h>


// info server
#define INFOSERVER "127.0.0.1"
#define INFOBUFLEN 1024
#define INFOPORT 8888


class StreamProcessor
{
public:
    StreamProcessor();
    ~StreamProcessor();

    void init(int id, char *iEth, char *iGroup, int iPort, char *oEth, char *oGroup, int oPort);
    void run();


private:

    struct sockaddr_in  inputSock;
    struct sockaddr_in  outputSock;
    struct ip_mreq      outputGroup;
    struct in_addr      outputInterface;
    int                 inputSocketDescriptor;
    int                 outputSocketDescriptor;

    int                 datalen;
    char                databuffer[1316];


    char *inputIP;
    char *outputIP;


    char *miEth;
    char *miGroup;
    int   miPort;

    char *moEth;
    char *moGroup;
    int   moPort;



    void initInformer();

    void initInput();
    void initOutput();

    char *interfaceToIP(char *eth);



    // Elapsed time
    struct timeval start, end;
    long mtime, seconds, useconds;

    int packetCount;
    int dataCount;


    // informer
    struct sockaddr_in si_other;
    int s, i, slen;

    int mID;

};

#endif // STREAMPROCESSOR_H
