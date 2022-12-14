#include <functional>
#include <string>

#include "chatserver.hpp"
#include "chatservice.hpp"
#include "json.hpp"

using namespace std;
using namespace placeholders;
using json =  nlohmann::json;

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg)
    : _server(loop, listenAddr, nameArg)
{
    // 注册连接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数量
    _server.setThreadNum(4);
}

// 启动服务器
void ChatServer::start(){
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr& conn){
    // 客户端已经断开连接
    if(!conn->connected()){
        ChatService::getInstance()->clientCloseException(conn);
        conn->shutdown();  // 关闭套接字的写端
    }
}

// 网络层 --> 业务层
// 业务层应该编写处理粘包问题的代码，从muduo库的buffer中取数据时，应根据前8字节判断一条数据的长度，读取并进行相应处理，muduo只进行数据收发，不处理业务
// conn:和客户端的连接，buff:收发数据的缓冲区，time收到数据的时间
void ChatServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time){
    string buff = buffer->retrieveAllAsString();
    // 数据的反序列化，将字符串转为json对象
    json js = json::parse(buff);
    // 我们这里不对msgid进行判断以及调用响应处理方法，这会导致网络模块代码和业务模块代码强耦合在一起
    // 我们这样处理：在业务模块给每一个msgid预置一个回调，根据msgid的值执行相应的回调函数，网络模块不可见
    // 这行代码获取msgid对应的方法，并调用
    auto handler = ChatService::getInstance()->getHandler(js["msgid"].get<int>());
    handler(conn, js, time);
}


// 每个loop都有一个线程，onMessage方法会有多个线程调用，即这些处理业务的回调方法都有多个线程调用，有一些数据成员的访问需要保证线程安全