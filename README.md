基于moduo实现的集群聊天服务器，用nginx实现负载均衡，用redis解耦服务器的硬连接，通过消息队列发布订阅的方式实现跨服务器聊天
并集成了数据库连接池，加快访问数据库的速度，以免每次访问都要创建连接，连接数据库

编译方式：
cd build
rm -rf *
cmake ..
make
