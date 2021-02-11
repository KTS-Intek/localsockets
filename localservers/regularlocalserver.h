#ifndef REGULARLOCALSERVER_H
#define REGULARLOCALSERVER_H

#include <QLocalServer>

class RegularLocalServer : public QLocalServer
{
    Q_OBJECT
public:
    explicit RegularLocalServer(const bool &verboseMode, QObject *parent = nullptr);

    int connCounter;
    bool verboseMode;


    virtual QString getPath2server();

signals:
    void append2log(QString message);//it adds date time automatically

public slots:
    void initLocalServer();

    void startServerLater(const int &msec);

    void onOneDisconn();

    void stopLocalServer();

    void kickOffLocalServer();

    void startServer();

};

#endif // REGULARLOCALSERVER_H
