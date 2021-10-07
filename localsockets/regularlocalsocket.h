#ifndef REGULARLOCALSOCKET_H
#define REGULARLOCALSOCKET_H

#include <QLocalSocket>
#include <QTime>
#include <QVariantHash>
#include <QVariantMap>


class RegularLocalSocket : public QLocalSocket
{
    Q_OBJECT
public:
    explicit RegularLocalSocket(const bool &verboseMode, QObject *parent = nullptr);
    bool activeDbgMessages;

    quint16 mtdExtName;
    QTime timeHalmo;
    quint8 zombieNow;
    bool stopAll, verboseMode;

    int inConn;

    virtual QString getPath2server();

    virtual QVariant getExtName();

    virtual void decodeReadData(const QVariant &dataVar, const quint16 &command);

    bool isConnectionStateUp();

    qint64 mWrite2extensionF(const QVariant &s_data, const quint16 &s_command);

signals:

    void connectionIsRegistered();

    void startReconnTmr();

    void startZombieKiller();
    void stopZombieKiller();

    void startZombieDetect();
    void stopZombieDetect();


    void onConfigChanged(quint16 command, QVariant datavar);

    void command4dev(quint16 command, QString args);//directly to scheduler, command is a pollcode,

//    void command4devHash(quint16 command, QVariantHash hash);//directly to scheduler, command is a pollcode,
#ifdef ENABLE_VERBOSE_SERVER
    void appendDbgExtData(quint32 sourceType, QString data);
#endif



public slots:
    //for client side
    void initializeSocket(quint16 mtdExtName);
    void command2extensionClient(quint16 command, QVariant dataVar);

    void connect2extension();
    void stopConnection();


    void onThreadStarted();

    void mWrite2extension(const QVariant &s_data, const quint16 &s_command);

    void sendAboutZigBeeModem(QVariantHash aboutModem);

    void killAllObjects();

    void forcedReading();

private slots:
    void mReadyRead();
    void onDisconn();

    void onZombie();



private:
    void mReadyReadF();

    void decodeReadDataF(const QVariant &dataVar, const quint16 &command);


};

#endif // REGULARLOCALSOCKET_H
