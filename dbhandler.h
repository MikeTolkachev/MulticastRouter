#ifndef DBHANDLER_H
#define DBHANDLER_H

#include <mysql/mysql.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

class StreamsInfo {
public:
    int id;
    char iEth[100];
    char iGroup[100];
    int iPort;
    char oEth[100];
    char oGroup[100];
    int oPort;
};

class ProviderInfo {
public:
    int  id;
    char ethName[100];
    char ethIP[100];
    char ethNETMASK[100];
    int  ethVLAN;
};



class DBHandler
{
public:
    DBHandler();
    ~DBHandler();
    void init();

    vector<StreamsInfo*>  *getStreamsList();
    vector<StreamsInfo*>  *getStreamsList(int id);
    vector<ProviderInfo*> *getProviderList();
    vector<ProviderInfo*> *getProviderList(int id);




private:
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    char *server;
    char *user;
    char *password;
    char *database;

    vector<StreamsInfo*>  *streams;
    vector<ProviderInfo*> *providers;

    void dbConnect();
    void prepareStreams();
    void prepareProviders();
};

#endif // DBHANDLER_H
