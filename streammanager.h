#ifndef STREAMMANAGER_H
#define STREAMMANAGER_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <map>
#include <string.h>

#include <string>
#include <sstream>
#include <fstream>

#include "streamprocessor.h"
#include "dbhandler.h"
#include "commandserver.h"

using namespace std;

enum commands {
    com_AddStream = 1,
    com_RemoveStream,
    com_AddInterface,
    com_RemoveInterface
};

class StreamManager
{
public:
    StreamManager();
    ~StreamManager();

    void init();
    void inputMessage(char *data, int datasize);


private:
//    pthread_t       *mReceiverThread;
    static void     *mReceiverWorker(void *arg);

    DBHandler       *mDBHandler;


    pthread_t       *mCommandServerThread;
    static void     *mCommandServerWorker(void *arg);

    map<int, pthread_t*> *processList;

    vector<string> split(char *data, int sz);


    void comAddStream(StreamsInfo *curStream);
    void comRemoveStream(int streamID);

    void comAddInterface(int opID);
    void comRemoveInterface(int opID);



};

#endif // STREAMMANAGER_H
