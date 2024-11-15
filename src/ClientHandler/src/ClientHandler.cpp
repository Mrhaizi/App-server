#include "ClientHandler.h"

ClientHandler::ClientHandler(qintptr descriptor, QObject *parent) 
    : QObject(parent), socket_(new QTcpSocket()), thread_(new QThread(this)) {
    // 关联套接字描述符，初始化 QTcpSocket
    if (!socket_->setSocketDescriptor(descriptor)) {
        qWarning() << "Failed to set socket descriptor!";
        delete socket_;
        socket_ = nullptr;
        return;
    }
    qDebug() << "Client connected from" << socket_->peerAddress().toString() << ":" << socket_->peerPort();
    socket_->moveToThread(thread_);
    connect(thread_, &QThread::started, this, &ClientHandler::run);
    thread_->start();
}
void ClientHandler::run() {
    // 连接信号和槽
    connect(socket_, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);  // 读取数据
}

void ClientHandler::onReadyRead() {
    QByteArray data = socket_->readAll();
    if (data.isEmpty()) {
        std::cout << "没有读取到数据" << std::endl;
    } else {
        std::cout << "成功读取到数据" << std::endl;
        handleClientData(data); // 处理数据
    }
}

void ClientHandler::handleClientData(const QByteArray &data) {
    QDataStream in(data);
    in.setVersion(QDataStream::Qt_5_12);  // 确保版本一致
    if (data.size() < sizeof(quint16) * 2) {
        std::cout << "数据太短" << std::endl;
    }
    quint16 messageLength;
    in >> messageLength; 
    quint16 func_id;
    in >> func_id;
    if (data.size() - sizeof(quint16) < messageLength) {
        std::cout << "data error" << std::endl;
    }
    QByteArray receive_data;
    in >> receive_data;   // 读取实际消息内容 
    // 输出或处理接收到的消息
    std::cout << "Received data: " << receive_data.toStdString() << " size: " << messageLength  << " func_id: 0x" << std::hex << func_id << std::dec << std::endl;
    if (func_id == 0x1a) {
        ChatMessage rec_msg = ChatMessage();
        rec_msg.castFromString(receive_data.toStdString());
        rec_msg.printMessage();
    }
}

void ClientHandler::onClientDisconnected() {
    socket_->deleteLater();
    thread_->quit();
    thread_->wait();
    thread_->deleteLater();
}

ClientHandler::~ClientHandler() {
    if (socket_->isOpen()) {
        socket_->close();
    }
    qDebug() << "ClientHandler destroyed";
}

