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
	int RecvMsg();      //接受消息
	int BroadCast();    //广播消息
	
	
private:
	static void* Consume(void*);     
	static void* Product(void*);      //pthread_create的 线程入口函数 是普通指针，不能把类的非静态指针传入。
	
	void AddUser(sockaddr_in addr,const std::string& name);
	void DelUser(sockaddr_in addr,const std::string& name);
	
	void SendMsg(const std::string& str,sockaddr_in addr);
	//key内容用户身份标识，ip+昵称，，，，ipd地址和端口号是在peer中，用户名在序列化里
	//value ip地址+端口号（struct sockaddr_in）
	std::unordered_map<std::string,sockaddr_in> online_friend_list_;    //_后缀标识私有成员
	//to do 实现一个队列作为交易场所，此处需要使用一个阻塞队列，作为生产者消费者模型。
	//BlockQueue<std::string> queue_;  //使用一个阻塞队列作为生产者消费的交易模型.
	BlockQueue<Context> queue_;
	int sock_;                       //服务器进行绑定的文件描述符,后续的文件读写，都得通过它来进行.
	};
}//end server 

//通过block传递，队列的简单扩充，即包含string，又包含sockaddr_in，
//或者把更新的代码放到recv函数中执行，两种方式.
//此处采用扩充队列,队列中的类型是可以随意扩充的.
