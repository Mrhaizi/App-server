#include "MyTcpServer.h"
#include "MessageBase.h"
#include "DataHandle.h"
#include "ChatMessage.h"

MyTcpServer::MyTcpServer() {

}
void MyTcpServer::handleClient(Zues::Socket::ptr client) {
    client->resetTimer();
    Zues::SocketStream::ptr client_stream = std::make_shared<Zues::SocketStream>(client, false); // 此处要特别注意如果为主控  会直接析构掉Socket 关掉client
    Zues::ByteArray::ptr ba = std::make_shared<Zues::ByteArray>(1024);
    static char buf[4096];  // 初始分配大小为 4096 字节
    TcpPacket tcp_packet;
    while (true) {
        // 清理缓冲区
        ba->clear();
        memset(&tcp_packet.body[0], 0, sizeof(tcp_packet.body));
        // 调整 buf 大小
        int rt = client_stream->read(ba, 4096);  // 读取数据
        if (rt > 0) {  // 读取成功               
            ba->setPosition(0);
            tcp_packet.length = ba->readUint32();
            ba->setPosition(4);
            tcp_packet.func_id = ba->readUint16();
            ba->setPosition(6);
            tcp_packet.version = ba->readUint8();
            ba->setPosition(7);
            tcp_packet.reserved = ba->readUint8();
            ba->setPosition(8);
            ba->read(&tcp_packet.body[0], tcp_packet.length);
            // 重新添加这个socket到IO事件调度当中, 等待下一次读取事件
            auto self = std::static_pointer_cast<MyTcpServer>(shared_from_this());
            DataHandleMachine::singleton::getInstance()->handleData(tcp_packet, client, self);
            Zues::IoManager::getThis()->scheduler(std::bind(&MyTcpServer::wathcIoRead, self, client));
            break;  // 跳出循环，等待新的事件来处
        } else if (rt == 0) {  // 连接关闭
            ZUES_LOG_STREAM_DEBUG(ZUES_GET_LOG("normal")) << "Client disconnected";
            client->close();
            break;  // 退出循环
        } else {  // 读取失败
            ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("normal")) << "Read error: " << strerror(errno);
            client->close();
            break;
        }
    }  
}
void MyTcpServer::wathcIoRead(Zues::Socket::ptr client) {
    if (!client->getSocket()) {
        ZUES_LOG_STREAM_DEBUG(ZUES_GET_LOG("normal")) << "watchIoRead error";
        return;
    }
    ZUES_LOG_STREAM_DEBUG(ZUES_GET_LOG("normal")) << "Re-adding read event for client socket" << "   socket :" << client->getSocket();
    auto self = std::static_pointer_cast<MyTcpServer>(shared_from_this());
    Zues::IoManager::getThis()->addEvent(client->getSocket(), Zues::IoManager::READ, std::bind(&MyTcpServer::handleClient, self, client));
}

void MyTcpServer::sendMessageToClient(Zues::Socket::ptr client, const std::string& send_message) {
    Zues::SocketStream::ptr client_stream = std::make_shared<Zues::SocketStream>(client, false); // 此处要特别注意如果为主控  会直接析构掉Socket 关掉client
    client_stream->write(send_message.c_str(), send_message.size());
}

void MyTcpServer::watchIoWrite(Zues::Socket::ptr client, const std::string& send_message) {
    if (!client->getSocket()) {
        ZUES_LOG_STREAM_DEBUG(ZUES_GET_LOG("normal")) << "watchIoWrite error";
        return;
    }
    // ZUES_LOG_STREAM_DEBUG(ZUES_GET_LOG("normal")) << "Re-adding read event for client socket" << "   socket :" << client->getSocket();
    auto self = std::static_pointer_cast<MyTcpServer>(shared_from_this());
    Zues::IoManager::getThis()->addEvent(client->getSocket(), Zues::IoManager::WRITE, std::bind(&MyTcpServer::sendMessageToClient, self, client, send_message));
}


void MyTcpServer::startAccpet(Zues::Socket::ptr socket)  {
    m_recvtimeout = (uint64_t)(60 * 1000 * 2);
    while(!m_isstop) {
        Zues::Socket::ptr client = socket->accept();
        if (client->ifConnected()) {
            Zues::SocketManager::singleton::getInstance()->addSocket(client);
            auto timer = m_io_worker->addTimer(100000, [=](){
                Zues::SocketManager::singleton::getInstance()->deleteSocket(client);
                UserConnectionManager::singleton::getInstance()->changeOnline(client);
            }, false); 
            client->addTimer(timer);
            client->setReceiveTimeout(m_recvtimeout);
            m_io_worker->addEvent(client->getSocket(), Zues::IoManager::READ, std::bind(&TcpServer::handleClient, shared_from_this(), client));
        } else {
            ZUES_LOG_STREAM_ERROR(ZUES_GET_LOG("normal")) << "scheduler error";
        }
    }
}
