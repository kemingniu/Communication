//chat_server.cc

#include"chat_server.h"
#include "../common/util.hpp"    
#include "api.hpp"
#include<sstream>


namespace server
{
    int ChatServer::Start(const std::string& ip,short port)
	{
		
		
		sock_ = socket(AF_INET,SOCK_DGRAM,0);
		if(sock_ < 0)
		{
			perror("socket");
			return -1;
		}
		sockaddr_in  addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
		addr.sin_port = htons(port);
		int ret = bind(sock_,(sockaddr*)&addr,sizeof(addr));
		if(ret < 0)
		{
			perror("bind");
			return -1;
		}
		LOG(INFO)<<"Server Start Ok!\n";
		
		
		
		pthread_t productor,consumer;
		pthread_create(&productor,NULL,Product,this);  
		pthread_create(&consumer,NULL,Consume,this);    
		pthread_join(productor,NULL);
		pthread_join(consumer,NULL);
		
		return 0;
	}
	
    void* ChatServer::Consume(void* arg)
	{
		ChatServer* server = reinterpret_cast<ChatServer*>(arg);
		while(true)
		{
			server->BroadCast();
		}
		return NULL;
	}		
	void* ChatServer::Product(void* arg)  
	{
		//生产者线程主要：读取socket中的数据，并且写入到BlockQueue中,
		ChatServer* server = reinterpret_cast<ChatServer*>(arg);
		while(true)
		{
			server->RecvMsg();
		}
		
		return NULL;
	}
	

	
	int ChatServer::RecvMsg()      //读取一次数据，并且写到BlockQueue中
	{
		//1.从socket中读取数据，
		char buf[1024*5] ={0};    //5k
		sockaddr_in peer;
		socklen_t len = sizeof(peer);
		ssize_t read_size = recvfrom(sock_,buf,sizeof(buf)-1,0,(sockaddr*)&peer,&len);
		if(read_size < 0)
		{
			perror("recvfrom");
			return -1;
		}
		buf[read_size] = '\0';
		LOG(INFO)<<"[Request]"<<buf<<"\n";
		//2.把数据插入到BlockQueue中。
		Context context;
		context.str = buf;
		context.addr = peer;
		queue_.Push_Back(context);     
		return 0;
	}
	
	
	int ChatServer::BroadCast()
	{
		//1.从队列中读取一个元素
		//std::string str;
		Context context;
		
		queue_.PopFront(&context);
		//2.对取出的字符串数据进行反序列化
		Data data;
		data.UnSerialize(context.str);
		//3.根据这个消息来更新在线好友列表，
		if(data.cmd == "quit")
		{
		//  2.如果这个成员当前是一个下线的消息，（command是一个特殊的值）
		//   把这个成员从好友列表中删除	
		
		DelUser(context.addr,data.name);
		}else{
		//  1.如果这个成员不再好友列表中，加进来，
		AddUser(context.addr,data.name);
		}
		//4.遍历在线好友，把这个数据以次发送给每一个客户端。
		//遍历unordered——map
		LOG(INFO)<<"=================================="<<"\n";
		for(auto item : online_friend_list_)
		{	
			SendMsg(context.str,item.second);
		LOG(INFO)<<"[Response]"<<inet_ntoa(context.addr.sin_addr)<<":"<<ntohs(context.addr.sin_port)<<" "<<data.msg<<"\n";
			
		}
		LOG(INFO)<<"========================================"<<"\n";
			
		return 0;
	}
	
	void ChatServer::AddUser(sockaddr_in addr,const std::string& name)
	{
		
		std::stringstream ss;
		// niuxiaoke:123456
		ss<<name<<":"<<addr.sin_addr.s_addr;
		std::string key; //端口号+ip地址，
		//sockaddr_in value = addr;
		online_friend_list_[ss.str()] = addr;
		return ;
		
	}
	void ChatServer::DelUser(sockaddr_in addr,const std::string& name)
	{
		
		std::stringstream ss;
		ss<< name << ":" <<addr.sin_addr.s_addr;
		online_friend_list_.erase(ss.str());
		
		return ;
	}
	
	void ChatServer::SendMsg(const std::string& data,sockaddr_in addr)
	{
		
		sendto(sock_,data.c_str(),data.size(),0,(sockaddr*)&addr,sizeof(addr));
		LOG(INFO)<<"[Response]"<<inet_ntoa(addr.sin_addr)<<":"<<ntohs(addr.sin_port)<<" "<<data<<"\n";
		
	}
	
}//end server


