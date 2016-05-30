#include "dbhandler.h"

DBHandler::DBHandler()
{
    server = "localhost";
    user = "user";
    password = "pass";
    database = "mroute_gw";
    streams = new vector<StreamsInfo*>;
    providers = new vector<ProviderInfo*>;
}

DBHandler::~DBHandler()
{

}

void DBHandler::init()
{

}

vector<StreamsInfo *> *DBHandler::getStreamsList()
{
    dbConnect();
    if (mysql_query(conn, "select mr.id as id, mi.interface as inint, mi.group as ingroup, mi.port as inport, mo.interface as outint, mo.group as outgroup, mo.port as outport from multicast_relations mr join multicast_in mi on mr.in=mi.id join multicast_out mo on mr.out=mo.id where mr.active=1;"))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }
    prepareStreams();
    return streams;
}

vector<StreamsInfo *> *DBHandler::getStreamsList(int id)
{
    dbConnect();

    string query = "select mr.id as id, mi.interface as inint, mi.group as ingroup, mi.port as inport, mo.interface as outint, mo.group as outgroup, mo.port as outport from multicast_relations mr join multicast_in mi on mr.in=mi.id join multicast_out mo on mr.out=mo.id where mr.id=";
    query.append(to_string(id));
    query.append(";");

    if (mysql_query(conn, query.c_str()))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }
    prepareStreams();
    return streams;
}

vector<ProviderInfo *> *DBHandler::getProviderList()
{
    dbConnect();

    string query = "select * from operators;";

    if (mysql_query(conn, query.c_str()))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }

    prepareProviders();
    return providers;
}

vector<ProviderInfo *> *DBHandler::getProviderList(int id)
{
    dbConnect();

    string query = "select * from operators where id=";
    query.append(to_string(id));
    query.append(";");

    if (mysql_query(conn, query.c_str()))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return NULL;
    }

    prepareProviders();
    return providers;
}

void DBHandler::dbConnect()
{
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, server, user, password, database, 3306, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }
}

void DBHandler::prepareStreams()
{
    streams->clear();
    res = mysql_use_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        StreamsInfo *mcurStream = new StreamsInfo();
        mcurStream->id = atoi(row[0]);
        strncpy(mcurStream->iEth, row[1], strlen(row[1])+ 1);
        strncpy(mcurStream->iGroup, row[2], strlen(row[2])+ 1);
        mcurStream->iPort = atoi(row[3]);
        strncpy(mcurStream->oEth, row[4], strlen(row[4])+ 1);
        strncpy(mcurStream->oGroup, row[5], strlen(row[5]) + 1);
        mcurStream->oPort = atoi(row[6]);
        streams->push_back(mcurStream);
    }

    mysql_free_result(res);
    mysql_close(conn);
}

void DBHandler::prepareProviders()
{
    providers->clear();
    res = mysql_use_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        ProviderInfo *mcurProvider = new ProviderInfo();
        mcurProvider->id = atoi(row[0]);
        strncpy(mcurProvider->ethName, row[1], strlen(row[1])+ 1);
        strncpy(mcurProvider->ethIP, row[2], strlen(row[2])+ 1);
        strncpy(mcurProvider->ethNETMASK, row[3], strlen(row[3])+ 1);
        mcurProvider->ethVLAN = atoi(row[4]);
        providers->push_back(mcurProvider);
    }

    mysql_free_result(res);
    mysql_close(conn);
}
