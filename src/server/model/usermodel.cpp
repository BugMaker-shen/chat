#include "usermodel.hpp"
#include "db.hpp"

// User表的增加方法，用户注册时需要使用
bool UserModel::insert(User& user){
    // 1. 组装SQL语句
    char sql[1024] = {0};
    // 此处刚注册，还没有登录，肯定是offline，我们在User类的构造函数里state默认为offline
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
        user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());
    
    // 2. 使用MySQL对象操作数据库
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            // 用户注册成功，则将用户对应的主键id添加到user对象
            int insert_id = mysql_insert_id(mysql.getConnection());
            user.setId(insert_id);
            return true;
        }
    }

    return false;
}

// User表的查询方法，用户登录时需要使用
User UserModel::query(int id){
    // 1. 组装SQL语句
    char sql[1024] = {0};
    // 此处刚注册，还没有登录，肯定是offline，我们在User类的构造函数里state默认为offline
    sprintf(sql, "select * from user where id=%d", id);
    
    // 2. 使用MySQL对象操作数据库
    MySQL mysql;
    if(mysql.connect()){
        // malloc申请了资源，需要释放
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr){
                User user;
                user.setId(atoi(row[0]));  // id
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                // 释放结果集占用的资源
                mysql_free_result(res);
                return user;
            }
        }
    }
    // 如果数据库没有连接上，或者没查找到id对应数据，则返回默认构造的user，id=-1
    return User();
}

// 登录成功后，更新数据库中的用户在线状态
bool UserModel::updateState(User user){
    // 1. 组装SQL语句
    char sql[1024] = {0};
    // 此处刚注册，还没有登录，肯定是offline，我们在User类的构造函数里state默认为offline
    sprintf(sql, "update user set state='%s' where id=%d", user.getState().c_str(), user.getId());
    
    // 2. 使用MySQL对象操作数据库
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    
    return false;
}