#include "streammanager.h"

StreamManager::StreamManager()
{

}

StreamManager::~StreamManager()
{

}

void StreamManager::init()
{
    processList = new map<int, pthread_t*> ;

    mDBHandler = new DBHandler();
    mDBHandler->init();

    mCommandServerThread = new pthread_t;
    pthread_create(mCommandServerThread, NULL, &mCommandServerWorker, this);

    vector<StreamsInfo*> *streams = mDBHandler->getStreamsList();
    for(uint ii=0; ii < streams->size(); ii++)
    {
        StreamsInfo *curStream = streams->at(ii);
        comAddStream(curStream);
    }
}

void StreamManager::inputMessage(char* data, int datasize)
{

    vector<string> curMessage = split(data, datasize);

    string commandName;

    if(curMessage.size() > 0)
    {
        commandName = curMessage.at(0);
    }

    uint commandID = atoi(commandName.c_str());

//    printf("COMMAND %d\n", commandID);

    switch(commandID)
    {

    case com_AddStream:
    {
        if(curMessage.size() == 2)
        {
            vector<StreamsInfo*> *streams = mDBHandler->getStreamsList(atoi(curMessage.at(1).c_str()));
            for(uint ii=0; ii < streams->size(); ii++)
            {
                StreamsInfo *curStream = streams->at(ii);
                comAddStream(curStream);
            }
        }
        break;
    }

    case com_RemoveStream:
    {
        if(curMessage.size() == 2)
        {
            comRemoveStream(atoi(curMessage.at(1).c_str()));
        }
        break;
    }

    case com_AddInterface:
    {
        if(curMessage.size() == 2)
        {
            comAddInterface(atoi(curMessage.at(1).c_str()));
        }
        break;
    }

    case com_RemoveInterface:
    {
        if(curMessage.size() == 2)
        {
            comRemoveInterface(atoi(curMessage.at(1).c_str()));
        }
        break;
    }

    default:
    {
        //        comUndefined(lastCommandID);
    }

    }


}

void *StreamManager::mReceiverWorker(void *arg)
{
    StreamsInfo *curStream = (StreamsInfo*)arg;

    //    printf("%s\n", curStream->oGroup);

    StreamProcessor *mStreamProcessor = new StreamProcessor();
    mStreamProcessor->init(curStream->id, curStream->iEth, curStream->iGroup, curStream->iPort, curStream->oEth, curStream->oGroup, curStream->oPort);

    mStreamProcessor->run();
    pthread_exit(NULL);
}

void *StreamManager::mCommandServerWorker(void *arg)
{
    StreamManager *mManager = (StreamManager*)arg;
    CommandServer *mCommandServer = new CommandServer();
    mCommandServer->init(mManager);
    mCommandServer->run();
    pthread_exit(NULL);
}

vector<string> StreamManager::split(char *data, int sz)
{
    char *enddata = "\n";
    strncpy(data + sz, enddata, 2);
    string s(data);

    char *delim = ":";
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, *delim))
    {
        tokens.push_back(item);
    }
    return tokens;
}

void StreamManager::comAddStream(StreamsInfo *curStream)
{
    pthread_t *mReceiverThread = new pthread_t;
    processList->insert(pair<int,pthread_t*>(curStream->id, mReceiverThread));
    pthread_create(mReceiverThread, NULL, &mReceiverWorker, curStream);
}

void StreamManager::comRemoveStream(int streamID)
{
    auto search = processList->find(streamID);
    if(search != processList->end())
    {
        pthread_t *curStream = search->second;
        pthread_cancel(*curStream);
        processList->erase(search);
    }
}

void StreamManager::comAddInterface(int opID)
{
    vector<ProviderInfo*> *providers = mDBHandler->getProviderList(opID);
    for(uint ii=0; ii < providers->size(); ii++)
    {
        ProviderInfo *curProviders = providers->at(ii);
        char fileContent[2048];
        string filename = "/etc/sysconfig/network-scripts/ifcfg-";
        filename.append(curProviders->ethName);

        // ifdown
        string stopETH = "/usr/sbin/ifdown ";
        stopETH.append(filename);
        stopETH.append("\n");
        system(stopETH.c_str());

        // remove file
        remove(filename.c_str());


        // write new

        int charSize = sprintf(fileContent,
                               "DEVICE=%s\n"
                               "ONBOOT=yes\n"
                               "DEVICETYPE=ovs\n"
                               "TYPE=OVSIntPort\n"
                               "BOOTPROTO=none\n"
                               "IPADDR=%s\n"
                               "NETMASK=%s\n"
                               "OVS_BRIDGE=swcore\n"
                               "OVS_OPTIONS=\"tag=%d\"\n"
                               "DEFROUTE=no\n",
                               curProviders->ethName,
                               curProviders->ethIP,
                               curProviders->ethNETMASK,
                               curProviders->ethVLAN);

        printf("%s", fileContent);

        ofstream ifcfgFile;
        ifcfgFile.open(filename.c_str());
        ifcfgFile << fileContent;
        ifcfgFile.close();

        // set permissions
        string perm = "chmod 777 ";
        perm.append(filename);
        perm.append("\n");
        system(perm.c_str());

        // ifup
        string startETH = "/usr/sbin/ifup ";
        startETH.append(filename);
        startETH.append("\n");
        system(startETH.c_str());
    }
}

void StreamManager::comRemoveInterface(int opID)
{
    // stop streams for this interface


    vector<ProviderInfo*> *providers = mDBHandler->getProviderList(opID);
    for(uint ii=0; ii < providers->size(); ii++)
    {
        ProviderInfo *curProviders = providers->at(ii);
        string filename = "/etc/sysconfig/network-scripts/ifcfg-";
        filename.append(curProviders->ethName);

        // ifdown
        string stopETH = "/usr/sbin/ifdown ";
        stopETH.append(filename);
        stopETH.append("\n");
        system(stopETH.c_str());

        // remove file
        remove(filename.c_str());
    }
}


