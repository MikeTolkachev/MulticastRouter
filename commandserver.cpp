#include "commandserver.h"
#include "streammanager.h"

CommandServer::CommandServer()
{

}

CommandServer::~CommandServer()
{

}

void CommandServer::init(StreamManager *manager)
{
    mManager = manager;

    socketDescriptor = -1;

    if ((socketDescriptor = socket( PF_INET, SOCK_DGRAM, 0 )) < 0)
    {
        printf("Problem creating socket\n");
        exit(1);
    }

    skaddr.sin_family = AF_INET;
//    skaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    skaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    skaddr.sin_port = htons(SERVERPORT);

    if (bind(socketDescriptor, (struct sockaddr *) &skaddr, sizeof(skaddr))<0)
    {
        printf("Problem binding\n");
        exit(0);
    }

    length = sizeof( skaddr );
    if (getsockname(socketDescriptor, (struct sockaddr *) &skaddr, &length)<0)
    {
        printf("Error getsockname\n");
        exit(1);
    }
}

void CommandServer::run()
{

    char bufin[MAXBUF];

    while (true)
    {
        packetSize = recvfrom(socketDescriptor, bufin, MAXBUF, 0, (struct sockaddr *)&clientaddr, &clientlen);
        if (packetSize < 0)
        {
            perror("Error receiving data");
        }
        else
        {
            mManager->inputMessage(bufin, packetSize);
        }
    }
}
