#include <iostream>
#include <QApplication>
#include <sstream>
#include <vector>
#include <nlohmann/json.hpp>

#include "CommunicatorServer.h"
#include "IoManager.h"
#include "Logger.h"
#include "MysqlConfigManager.h"
#include "MyFiber.h"
#include "Scheduler.h"
#include "MyThread.h"
#include "Timer.h"
#include "TcpServer.h"
#include "SocketStream.h"
#include "ByteArray.h"
#include "MyTcpServer.h" 



void run() {
    MyTcpServer::ptr server(new MyTcpServer());
    auto addr = Zues::Address::LookUpAny("0.0.0.0:12345");
    std::vector<Zues::Address::ptr> addrs;
    addrs.push_back(addr);
    std::vector<Zues::Address::ptr> fails;
    while(!server->bind(addrs, fails)) {
        sleep(2);
    }
    server->start();
}


int main(int argc, char *argv[])
{
    Zues::LogManager::init("/home/myq/Desktop/my_code/App-server/conf/log.yaml");
    SQL_SET_IP("tcp://127.0.0.1:3306");
    SQL_SET_NAME("root");
    SQL_SET_PASSWORD("myq");
    SQL_SET_DATABASE("app_info");
    Zues::IoManager iom(20, false, "IOM2");
    iom.scheduler(&run);
    while (1)
    {
        /* code */
    }
    return 0;
}
