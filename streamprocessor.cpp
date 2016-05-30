#include "streamprocessor.h"

StreamProcessor::StreamProcessor()
{
    //    isRunning = true;
}

StreamProcessor::~StreamProcessor()
{

}

void StreamProcessor::init(int id, char *iEth, char *iGroup, int iPort, char *oEth, char *oGroup, int oPort)
{
    mID = id;
    miEth = iEth;
    miGroup = iGroup;
    miPort = iPort;
    moEth = oEth;
    moGroup = oGroup;
    moPort = oPort;

    initInformer();
    initInput();
    initOutput();
}

void StreamProcessor::run()
{
    packetCount = 0;
    dataCount = 0;
    gettimeofday(&start, NULL);

    int nbytes = 0;
    socklen_t addrlen=sizeof(inputSock);

    while(true)
    {
        if ((nbytes=recvfrom(inputSocketDescriptor,databuffer, 1316, 0, (struct sockaddr*)&inputSock, &addrlen) < 0))
        {
            gettimeofday(&end, NULL);
            seconds  = end.tv_sec  - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;
            mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
        }
        else
        {
            packetCount++;

            sendto(outputSocketDescriptor, databuffer, datalen, 0, (struct sockaddr*)&outputSock, sizeof(outputSock));
            gettimeofday(&end, NULL);
            seconds  = end.tv_sec  - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;
            mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

            // count FF packets from end to start

            for (int i = 0; i < datalen; i++)
            {
                if(((unsigned)(unsigned char)databuffer[i]) == 255)
                {
                    dataCount++;
                }
            }
        }


        if(mtime >= 1000)
        {
            char message[1000];
            int strSize = sprintf(message, "%d:%ld:%d\0", mID, packetCount*1316*8, dataCount*8);

            sendto(s, message, strSize , 0 , (struct sockaddr *) &si_other, slen);
            packetCount = 0;
            dataCount = 0;
            gettimeofday(&start, NULL);

        }
    }
}

void StreamProcessor::initInformer()
{
    slen=sizeof(si_other);
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("socket\n");
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(INFOPORT);

    if (inet_aton(INFOSERVER , &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
}

void StreamProcessor::initInput()
{
    if ((inputSocketDescriptor=socket(AF_INET,SOCK_DGRAM,0)) < 0)
    {
        perror("socket");
        exit(1);
    }



    char loopch = 0;
    if(setsockopt(inputSocketDescriptor, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
    {
        perror("Setting IP_MULTICAST_LOOP error");
        exit(1);
    }



    u_int reuse = 1;
    if (setsockopt(inputSocketDescriptor,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)) < 0)
    {
        perror("Reusing ADDR failed");
        exit(1);
    }

    struct timeval timeOut;
    timeOut.tv_sec = 0;
    timeOut.tv_usec = 100000;
    if (setsockopt(inputSocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,&timeOut,sizeof(timeOut)) < 0)
    {
        perror("Error");
    }

    // TTL
    socklen_t ttl = 255;
    if (setsockopt(inputSocketDescriptor, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0)
    {
        perror("Error");
    }

    memset((char *) &inputSock, 0, sizeof(inputSock));
    inputSock.sin_family = AF_INET;
    inputSock.sin_addr.s_addr = inet_addr(miGroup);
    inputSock.sin_port = htons(miPort);


    if(bind(inputSocketDescriptor, (struct sockaddr*)&inputSock, sizeof(inputSock)) < 0)
    {
        perror("bind");
        exit(1);
    }

    outputGroup.imr_multiaddr.s_addr = inet_addr(miGroup);
    outputGroup.imr_interface.s_addr = INADDR_ANY;

    if(setsockopt(inputSocketDescriptor, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&outputGroup, sizeof(outputGroup)) < 0)
    {
        perror("Adding multicast group error");
        exit(1);
    }



    datalen = sizeof(databuffer);
}

void StreamProcessor::initOutput()
{
    outputSocketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(outputSocketDescriptor < 0)
    {
        perror("Opening datagram socket error");
        exit(1);
    }

    memset((char *) &outputSock, 0, sizeof(outputSock));
    outputSock.sin_family = AF_INET;
    outputSock.sin_addr.s_addr = inet_addr(moGroup);
    outputSock.sin_port = htons(moPort);
    outputInterface.s_addr = inet_addr(interfaceToIP(moEth));
    if(setsockopt(outputSocketDescriptor, IPPROTO_IP, IP_MULTICAST_IF, (char *)&outputInterface, sizeof(outputInterface)) < 0)
    {
        perror("Setting local interface error");
        exit(1);
    }

    // TTL
    socklen_t ttl = 255;
    if (setsockopt(outputSocketDescriptor, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0)
    {
        perror("Error");
    }
}

char *StreamProcessor::interfaceToIP(char *eth)
{
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, eth, IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}
