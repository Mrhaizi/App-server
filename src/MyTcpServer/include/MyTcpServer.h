#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "IoManager.h"
#include "Logger.h"
#include "MyFiber.h"
#include "Scheduler.h"
#include "MyThread.h"
#include "Timer.h"
#include "TcpServer.h"
#include "SocketStream.h"
#include "ByteArray.h"
#include "SocketManage.h"


class MyTcpServer : public Zues::TcpServer{
public:
    typedef std::shared_ptr<MyTcpServer> ptr;
    MyTcpServer();
    void startAccpet(Zues::Socket::ptr socket) override;
    void handleClient(Zues::Socket::ptr client) override;
    void sendMessageToClient(Zues::Socket::ptr client, const std::string& send_message);
    void wathcIoRead(Zues::Socket::ptr client);
    void watchIoWrite(Zues::Socket::ptr client, const std::string& string);
    void deSerialization();
};
#endif //MYTCPSERVER_H 
