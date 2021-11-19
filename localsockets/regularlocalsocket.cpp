#include "regularlocalsocket.h"
#include <QDebug>
#include <QTimer>
#include <QDataStream>

///[!] MatildaIO
#include "matilda-bbb-src/shared/pathsresolver.h"
//#include "src/matilda/settloader4matilda.h"

//---------------------------------------------------------------------------------------

RegularLocalSocket::RegularLocalSocket(const bool &verboseMode, QObject *parent) : QLocalSocket(parent)
{
    this->verboseMode = verboseMode;//extended out / verbouseMode
#ifdef DISABLE_LOCALSOCKETVERBOSE
    this->verboseMode = false; //it is disabled for test only
#endif
    stopAll = false;
}

//---------------------------------------------------------------------------------------

QString RegularLocalSocket::getPath2server()
{
    return PathsResolver::defLocalServerName();
}

//---------------------------------------------------------------------------------------

QVariant RegularLocalSocket::getExtName()
{
    return QVariant(mtdExtName);
}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::decodeReadData(const QVariant &dataVar, const quint16 &command)
{
    qDebug() << "RegularLocalSocket::decodeReadData you did'n inherit this function " << dataVar << command;
}

bool RegularLocalSocket::isConnectionStateUp()
{
    return (state() == QLocalSocket::ConnectedState);

}

qint64 RegularLocalSocket::mWrite2extensionF(const QVariant &s_data, const quint16 &s_command)
{
    if(stopAll){
        if(verboseMode)
            qDebug() << "w stop " << stopAll;
        return -2;
    }

    if(!isConnectionStateUp()){
        if(verboseMode)
            qDebug() << "w !state " << state();
        emit startReconnTmr();
        return -1;
    }
#ifdef ENABLE_VERBOSE_SERVER
    if(activeDbgMessages)
        emit appendDbgExtData(DBGEXT_THELOCALSOCKET, QString("command w: %1, data=%2").arg(s_command).arg(s_data.toString()));
#endif
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_6);
    out << (quint32)0;
    out << s_command << s_data;
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    const qint64 r = write(block);
    waitForBytesWritten(50);
    timeHalmo.restart();
    return r;
}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::initializeSocket(quint16 mtdExtName)
{
    this->mtdExtName = mtdExtName;

}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::command2extensionClient(quint16 command, QVariant dataVar)
{
    if(verboseMode)
        qDebug() << "RegularLocalSocket::command2extensionClient " << command << dataVar;

    mWrite2extension(dataVar, command);
}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::connect2extension()
{
    emit stopZombieKiller();
    emit stopZombieDetect();

    if(state() != QLocalSocket::UnconnectedState){
        close();
        waitForDisconnected();
    }
    stopAll = false;
    zombieNow = 0;

    const QString path2server = getPath2server();

    if(!path2server.isEmpty())
        connectToServer(path2server);

    if(waitForConnected(500)){
#ifdef HASGUI4USR
        if(!verboseMode)
        qDebug() << "connect2extension works" << errorString() << state() << mtdExtName;

#endif

        emit startZombieDetect();
        if(verboseMode)
            qDebug() << "connect2extension works" << path2server << errorString() << state() << mtdExtName;
        return;
    }
    if(verboseMode )
        qDebug() << "connect2extension not working " << path2server << errorString() << state() << mtdExtName;

    stopAll = true;
    emit startReconnTmr();
}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::stopConnection()
{
    stopAll = true;
    close();
}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::onThreadStarted()
{
    connect(this, SIGNAL(readyRead()), this, SLOT(mReadyRead()) );
    connect(this, SIGNAL(disconnected()), this, SLOT(onDisconn()) );

    QTimer *zombieDetect = new QTimer(this);
    zombieDetect->setSingleShot(true);
    zombieDetect->setInterval(9999);// 10 сек

    connect(this, SIGNAL(startReconnTmr()), zombieDetect, SLOT(stop()) );
    connect(this, SIGNAL(startZombieDetect()), zombieDetect, SLOT(start()) );
    connect(this, SIGNAL(stopZombieDetect()), zombieDetect, SLOT(stop()) );
    connect(zombieDetect, SIGNAL(timeout()), this, SLOT(onZombie()) );


    QTimer *voskresator = new QTimer(this);
    voskresator->setSingleShot(true);
    voskresator->setInterval(333);

    connect(this, SIGNAL(startReconnTmr()), voskresator, SLOT(start()) );
    connect(this, SIGNAL(startZombieKiller()), voskresator, SLOT(stop()) );
    connect(this, SIGNAL(stopZombieKiller()), voskresator, SLOT(stop()) );
    connect(this, SIGNAL(stopZombieDetect()), voskresator, SLOT(stop()) );
    connect(this, SIGNAL(startZombieDetect()), voskresator, SLOT(stop()) );
    connect(this, SIGNAL(readyRead()), voskresator, SLOT(stop()) );

    connect(voskresator, SIGNAL(timeout()), this, SLOT(connect2extension()) );

    connect2extension();
}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::mWrite2extension(const QVariant &s_data, const quint16 &s_command)
{
    mWrite2extensionF(s_data, s_command);

}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::sendAboutZigBeeModem(QVariantHash aboutModem)
{
    if(verboseMode)
        qDebug() << "sendAboutZigBeeModem " << aboutModem;
    mWrite2extension(aboutModem, MTD_EXT_ABOUT_ZB);

}

void RegularLocalSocket::killAllObjects()
{
    stopConnection();

    deleteLater();

}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::forcedReading()
{
     mReadyReadF();
}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::mReadyRead()
{
    disconnect(this, SIGNAL(readyRead()), this, SLOT(mReadyRead()) );
    mReadyReadF();
    connect(this, SIGNAL(readyRead()), this, SLOT(mReadyRead()) );
}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::onDisconn()
{
    if(verboseMode)
        qDebug() << "onDisconn";
    close();
    stopAll = true;
    emit startReconnTmr();
}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::onZombie()
{
    zombieNow++;
    mWrite2extension(QByteArray("h"), MTD_EXT_PING_2_SERV);
    emit startZombieKiller();

    if(zombieNow > 3){
        onDisconn();
    }
}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::mReadyReadF()
{
    if(!isConnectionStateUp()){
        QTimer::singleShot(11, this, SLOT(onDisconn()));
        if(verboseMode)
            qDebug() << "r !state " << state();

        return;
    }

    zombieNow = 0;
    for(int i = 0; i < 50 && timeHalmo.elapsed() < 35; i++)
        waitForReadyRead(5);
    timeHalmo.restart();

    quint32 blockSize;
    QDataStream inStrm(this);
    quint16 serverCommand;

    inStrm.setVersion(QDataStream::Qt_5_6);

    if(bytesAvailable() < (qint64)sizeof(quint32))
        return;

    inStrm >> blockSize;

    QTime timeG;
    timeG.start();

    const int timeOutG = 9999;
    const int timeOut = 300;

    while(bytesAvailable() < blockSize && timeG.elapsed() < timeOutG){

        if(waitForReadyRead(timeOut) && verboseMode)
            qDebug()<< "readServer1:"<< bytesAvailable() << blockSize;
    }


    if(bytesAvailable() < blockSize || blockSize > MAX_PACKET_LEN || bytesAvailable() > MAX_PACKET_LEN){
        if(verboseMode)
            qDebug()<< "readServer:" << blockSize << bytesAvailable() << readAll().toHex();
        else
            readAll();
        return;
    }


    if(bytesAvailable() == blockSize){
        QVariant readVar;
        inStrm >> serverCommand >> readVar;
        decodeReadDataF(readVar, serverCommand);

    }else{
        if(!inStrm.atEnd()){
            QVariant readVar;
            inStrm >> serverCommand >> readVar;
            decodeReadDataF(readVar, serverCommand);
            QTimer::singleShot(11, this, SLOT(mReadyRead()) );
            return;
        }
    }
}

//---------------------------------------------------------------------------------------

void RegularLocalSocket::decodeReadDataF(const QVariant &dataVar, const quint16 &command)
{
    //it decodes staff commands, other - sends to a child class
#ifdef ENABLE_VERBOSE_SERVER
    if(activeDbgMessages)
        emit appendDbgExtData(DBGEXT_THELOCALSOCKET, QString("decodeReadData r: %1").arg(command));
#endif
    bool commandIsFound = true;
    switch(command){

    case MTD_EXT_GET_INFO: {
        mWrite2extension(getExtName(), MTD_EXT_GET_INFO );
        if(verboseMode) qDebug() << "ext " << mtdExtName << dataVar;
        emit connectionIsRegistered();

        break;}

    case MTD_EXT_GET_LOCATION: {
        if(verboseMode) qDebug() << "ext " << mtdExtName << dataVar;
        break;}

    case MTD_EXT_PING: {
        emit startZombieDetect();
        emit stopZombieKiller();
        break;}
    default: commandIsFound = false;

    }
    if(commandIsFound)
        return;

    decodeReadData(dataVar, command);
}

//---------------------------------------------------------------------------------------
