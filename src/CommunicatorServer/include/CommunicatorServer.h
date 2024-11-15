#ifndef __COMMUNICATOR_SERVER_H__
#define __COMMUNICATOR_SERVER_H__

#include <QTcpServer>
#include <QThreadPool>

#include "LogManager.h"

class CommunicatorServer : public QTcpServer 
{
    Q_OBJECT
public:
    explicit CommunicatorServer(QTcpServer *parent = nullptr, const int &port = -1);
    ~CommunicatorServer();

protected:
    void incomingConnection(qintptr socketDescriptor) override;  // override表示重载虚函数
};

#endif // __COMMUNICATOR_SERVER_H__

