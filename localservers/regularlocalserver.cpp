#include "regularlocalserver.h"

#include <QTimer>
#include <QDebug>


RegularLocalServer::RegularLocalServer(const bool &verboseMode, QObject *parent) : QLocalServer(parent)
{
    this->verboseMode = verboseMode;

}

QString RegularLocalServer::getPath2server()
{
    return QString();//

}

void RegularLocalServer::initLocalServer()
{

    const QString path2server = getPath2server();
    if(!path2server.isEmpty())
        QLocalServer::removeServer(path2server);

    connCounter = 0;


    if(verboseMode)
        qDebug() << "start local server"  << path2server;
}

void RegularLocalServer::startServerLater(const int &msec)
{
    QTimer::singleShot(msec, this, SLOT(startServer()) );

}

void RegularLocalServer::onOneDisconn()
{
    connCounter--;
    if(connCounter < 0 )
        connCounter = 0;

    if(connCounter > 50){
        if(verboseMode)
            qDebug() << "it is impossible RegularLocalServer connCounter=" << connCounter;
        //send command to restart application

    }
    if(verboseMode)
        qDebug() << "RegularLocalServer connCounter = " << connCounter;
}

void RegularLocalServer::stopLocalServer()
{
    close();
    const QString path2server = getPath2server();
    if(!path2server.isEmpty())
        QLocalServer::removeServer(path2server);// PathsResolver::defLocalServerName());
}

void RegularLocalServer::kickOffLocalServer()
{
    stopLocalServer();
    deleteLater();
}

void RegularLocalServer::startServer()
{


    if(isListening()){
        if(verboseMode)
            qDebug() << "RegularLocalServer server is listenn " << errorString();
        return;
    }

    const QString path2server = getPath2server();

    if(!path2server.isEmpty())
        QLocalServer::removeServer(path2server);

    if(path2server.isEmpty() || !listen(path2server)){
        if(verboseMode)
            qDebug() << "RegularLocalServer can't start local server " << path2server << errorString() ;
        if(!path2server.isEmpty())
            QLocalServer::removeServer(path2server);

        startServerLater(500);
        return;
    }

    if(verboseMode)
        qDebug() << "RegularLocalServer startServer " << isListening() << serverName() << errorString();
}
