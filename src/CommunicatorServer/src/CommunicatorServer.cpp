#include "CommunicatorServer.h"
#include "ClientHandler.h"
#include "LogManager.h"
#include <QDebug>

// 构造函数：初始化服务器，开始监听指定端口
CommunicatorServer::CommunicatorServer(QTcpServer *parent, const int &port) 
    : QTcpServer(parent) {
    if (!this->listen(QHostAddress::Any, port)) {
        ZUES_LOG_FMT_FATAL(ZUES_GET_LOG("system"), "Server could not start on %d", port);
    } else {
        ZUES_LOG_FMT_INFO(ZUES_GET_LOG("normal"), "Server started on %d", port);
    }
}
// 当有新连接时调用
void CommunicatorServer::incomingConnection(qintptr socketDescriptor) {
    std::cout << "new connect " << std::endl;
    ClientHandler *handler = new ClientHandler(socketDescriptor);  // 创建处理客户端的对象
}
// 析构函数：确保所有线程池任务完成后才销毁服务器
CommunicatorServer::~CommunicatorServer() {
    QThreadPool::globalInstance()->waitForDone();  // 确保线程池任务完成
}

