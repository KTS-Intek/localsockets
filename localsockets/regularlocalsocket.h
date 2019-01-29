#ifndef REGULARLOCALSOCKET_H
#define REGULARLOCALSOCKET_H

#include <QLocalSocket>

class RegularLocalSocket : public QLocalSocket
{
    Q_OBJECT
public:
    explicit RegularLocalSocket(bool verboseMode, QObject *parent = nullptr);
    bool activeDbgMessages;

    quint16 mtdExtName;
    QTime timeHalmo;
    quint8 zombieNow;
    bool stopAll, verboseMode;

    int inConn;

    virtual void decodeReadData(const QVariant &dataVar, const quint16 &command);

signals:

    void startReconnTmr();

    void startZombieKiller();
    void stopZombieKiller();

    void startZombieDetect();
    void stopZombieDetect();


    void onConfigChanged(quint16, QVariant);

    void command4dev(quint16 command, QString args);


    void appendDbgExtData(quint32 sourceType, QString data);

public slots:
    //for client side
    void initializeSocket(quint16 mtdExtName);
    void command2extensionClient(quint16 command, QVariant dataVar);

    void connect2extension();
    void stopConnection();


    void onThreadStarted();

private slots:
    void mReadyRead();
    void mWrite2extension(const QVariant &s_data, const quint16 &s_command);
    void onDisconn();

    void onZombie();



private:
    void mReadyReadF();


};

#endif // REGULARLOCALSOCKET_H
