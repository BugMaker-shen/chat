#ifndef PUBLIC_H
#define PUBLIC_H

/*
    server和client的公共文件
*/

enum EnMsgType{
    LOGIN_MSG,      // 登录消息
    LOGIN_MSG_ACK,
    REG_MSG,        // 注册消息
    REG_MSG_ACK,    // 注册相应消息
};

#endif