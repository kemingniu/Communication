//client目录
//chat_client.h

#pragma once

#include<string>

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
//#include<jsoncpp/json/json.h>
#include "../server/api.hpp"


namespace client{

class ChatClient{
public:

	//客户端初始化，
	int Init(const std::string& ip,short port);
	
	//设置用户信息,姓名 学校,客户端启动时，提示用户输入，从标准输入读取，传给下面的函数
	int SetUserInfo(const std::string& name,const std::string& school);
	//发送消息
	void SendMsg(const std::string& msg);
	//接受消息，接受的内容也是json格式的内容。
	void RecvMsg(server::Data* data); 
	


private:
	//通过成员变量，保存和服务器进行交互的socket
	//通过init函数创建好，
	int sock_;
	sockaddr_in server_addr_;
	std::string name_;
	std::string school_;
};


}//end of client
