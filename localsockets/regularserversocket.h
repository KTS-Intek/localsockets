#ifndef REGULARSERVERSOCKET_H
#define REGULARSERVERSOCKET_H


#include <QLocalSocket>
#include <QtCore>


class RegularServerSocket : public QLocalSocket
{
    Q_OBJECT
public:
    explicit RegularServerSocket(const bool &verboseMode, QObject *parent = nullptr);

    bool verboseMode;
    quint8 errCounter;

    QString mtdExtNameTxt;


    virtual quint16 getZombieCommand();


signals:
    void onConnectionLost();// startReconnTmr();


    //internal
    void startZombieKiller();
    void stopZombieKiller();

    void startZombieDetect();
    void stopZombieDetect();

    //external
    void append2log(QString message);

    //to inherited
    void onReadData(QVariant dataVar, quint16 command);



public slots:
    void initObjects();

    void stopConnection();


    void mReadyRead();

    void mWrite2extension(const QVariant &s_data, const quint16 &s_command);

    void onDisconn();

    void onZombie();


private:
    void mReadyReadF();


};

#endif // REGULARSERVERSOCKET_H
