#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <iostream>
#include <fstream>
#include <filesystem>

#include <QObject> 
#include <QThread>
#include <QTcpSocket>
#include <QRunnable>
#include <QDataStream>
#include <QDebug>
#include <QHostAddress>
#include <strstream>
#include "MysqlInteractor.h"
#include "ChatMessage.h"

class ClientHandler : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit ClientHandler(qintptr descriptor, QObject *parent = nullptr);
    ~ClientHandler();
    void run() override;
private slots:
    void onReadyRead();
    void handleClientData(const QByteArray &data);
    void onClientDisconnected();
signals:
    void finished();
private:
    QTcpSocket *socket_;
    QThread *thread_;
};
#endif // CLIENTHANDLER_H
