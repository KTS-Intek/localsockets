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

    void mWrite2extensionLater(const QVariant &s_data, const quint16 &s_command);

    void onDisconn();

    void onZombie();

    void onPing2serverReceived();

    void onPingReceived();

private slots:
    void freeWriteLater();

private:
    void mReadyReadF();

    struct MWriteLater
    {
        QVariant s_data;
        quint16 s_command;
        bool hasData;
        MWriteLater() : hasData(false) {}
        MWriteLater(const QVariant &s_data, const quint16 &s_command) : s_data(s_data), s_command(s_command), hasData(true) {}
    } writelater;

};

#endif // REGULARSERVERSOCKET_H
