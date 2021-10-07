#include "regularserversocket.h"

///[!] MatildaIO
#include "src/shared/readwriteiodevice.h"


#include "ifaceconnectiondefs.h"


RegularServerSocket::RegularServerSocket(const bool &verboseMode, QObject *parent) : QLocalSocket(parent)
{
#ifdef DISABLE_LOCAL_SOCKET_VERBOSE_MODE
    Q_UNUSED(verboseMode);
    this->verboseMode = false;
#else
    this->verboseMode = verboseMode;
#endif
    errCounter = 0;
}

quint16 RegularServerSocket::getZombieCommand()
{
    return 0;//there is no command

}

void RegularServerSocket::initObjects()
{
    QTimer *zombieTimer = new QTimer(this);
    zombieTimer->setSingleShot(true);
    zombieTimer->setInterval(5 * 60 * 1000);//5 хвилин
    connect(this, SIGNAL(readyRead()), zombieTimer, SLOT(start()) );
    connect(this, SIGNAL(startZombieDetect()), zombieTimer, SLOT(start()) );
    connect(this, SIGNAL(stopZombieDetect()), zombieTimer, SLOT(stop()) );
    connect(zombieTimer, SIGNAL(timeout()), SLOT(onDisconn()) );
    emit startZombieDetect();

    connect(this, SIGNAL(readyRead()), this, SLOT(mReadyRead()) );
    connect(this, SIGNAL(disconnected()), this, SLOT(onDisconn()) );

}

void RegularServerSocket::stopConnection()
{
    close();
}

void RegularServerSocket::mReadyRead()
{
    disconnect(this, SIGNAL(readyRead()), this, SLOT(mReadyRead()) );
    mReadyReadF();
    connect(this, SIGNAL(readyRead()), this, SLOT(mReadyRead()) );

    freeWriteLater();
}

void RegularServerSocket::mWrite2extension(const QVariant &s_data, const quint16 &s_command)
{
    if(state() != QLocalSocket::ConnectedState){
        emit onConnectionLost();// startReconnTmr();
        qDebug() << "!state " << state()<< mtdExtNameTxt;
        onDisconn();
        return;
    }

//    if(verboseMode && s_command != getZombieCommand()) a lot of data
//        qDebug() << "mWrite2extension " << s_command << s_data<< mtdExtNameTxt;

     ReadWriteIODevice::mWrite2Socket(this, s_data, s_command, verboseMode, NET_SPEED_UFS_1);//   //     qDebug() << block.toHex();

//    QByteArray block;
//    QDataStream out(&block, QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_5_6);
//    out << (quint32)0;
//    out << s_command << s_data;
//    out.device()->seek(0);
//    out << (quint32)(block.size() - sizeof(quint32));
//    write(block);
//    waitForBytesWritten(50);

}

void RegularServerSocket::mWrite2extensionLater(const QVariant &s_data, const quint16 &s_command)
{
    writelater = MWriteLater(s_data, s_command);
}

void RegularServerSocket::onDisconn()
{
    if(verboseMode)
        qDebug() << "onDisconn " << mtdExtNameTxt;
    close();
    deleteLater();
}

void RegularServerSocket::onZombie()
{

    const quint16 command = getZombieCommand();
    if(command > 0)
        mWrite2extension(QByteArray("h"), command);// MTD_EXT_PING);
    emit startZombieKiller();
}

void RegularServerSocket::onPing2serverReceived()
{
    onZombie();
}

void RegularServerSocket::onPingReceived()
{
    emit startZombieDetect();
    emit stopZombieKiller();
    if(verboseMode)
        qDebug() << "ping";
}

void RegularServerSocket::freeWriteLater()
{
    if(writelater.hasData){
        writelater.hasData = false;
        mWrite2extension(writelater.s_data, writelater.s_command);
    }
}

void RegularServerSocket::mReadyReadF()
{
    if(state() != QLocalSocket::ConnectedState){
        QTimer::singleShot(11, this, SLOT(onDisconn()));
        qDebug() << "!r state " << state()<< mtdExtNameTxt;
        return;
    }

    bool hasErr, hasMoreData;
    quint16 serverCommand = 0;
    quint32 blockSize = 0;
    const int timeOut = 500;
    const int timeOutG = 9000;

    const QVariant readVar = ReadWriteIODevice::readIODevice(this, timeOut, timeOutG, hasErr, hasMoreData, serverCommand, blockSize);

    if(hasErr){
        readAll();
        errCounter++;
        if(errCounter > 10)
            QTimer::singleShot(11, this, SLOT(onDisconn()));

        return;
    }

    errCounter = 0;

    emit onReadData(readVar, serverCommand);
    if(hasMoreData)
        QTimer::singleShot(11, this, SLOT(mReadyRead()) );
}
