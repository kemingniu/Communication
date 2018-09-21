//chat_server.h

#pragma once
#include<string>
#include<unordered_map>  //key显示身份信息(ip（ip可能会相同）或者昵称)，value
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include "block_queue.hpp"
#include "api.hpp"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;


namespace server{

struct Context{
	sockaddr_in addr;
	std::string str;
};

class ChatServer{
public:
	int Start(const std::string& ip,short port);
	int RecvMsg();      //接收消息
	int BroadCast();    //广播消息
	
	
private:
	static void* Consume(void*);     
	static void* Product(void*);      
	
	void AddUser(sockaddr_in addr,const std::string& name);
	void DelUser(sockaddr_in addr,const std::string& name);
	
	void SendMsg(const std::string& str,sockaddr_in addr);
	//key内容用户身份标识，ip+昵称，，，，ip地址和端口号是在peer中，用户名在序列化里
	//value ip地址+端口号（struct sockaddr_in）
	std::unordered_map<std::string,sockaddr_in> online_friend_list_;    
	
	
	BlockQueue<Context> queue_;
	int sock_;                       
	};
}//end server 
