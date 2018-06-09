//chat_server.cc

#include"chat_server.h"
#include "../common/util.hpp"    //  ../相对目录，
#include "api.hpp"
#include<sstream>


namespace server
{
    int ChatServer::Start(const std::string& ip,short port)
	{
		//启动服务器，进入时间循环
		//使用UDP作为我们的通信协议
		
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
		//这里想使用以前的打印日志函数， cp ../../lesson36/http_server/util.hpp
		//客户端和服务端都会都会用到，所以建立一个common文件夹，包含util.hpp，包含common目录中的文件就可以.
		//server client common
		pthread_t productor,consumer;
		pthread_create(&productor,NULL,Product,this);   //线程的入口函数，chatserver的对象，socket和队列
		pthread_create(&consumer,NULL,Consume,this);    //
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
	void* ChatServer::Product(void* arg)  //线程的入口函数中return NULL，线程就终止了。
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
		//如果已经在链表中，使用[]方法来操作，不会出错
		
		AddUser(context.addr,data.name);
		}
		//4.遍历在线好友，把这个数据一次发送给每一个客户端。（由于发送消息的用户，也存在与列表中，在遍历列表式，也会给发送自己发送消息
		// 完全可以不经过客户端，不经过网络传输就实现这个功能，）
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
		//依据key value的约定，
		//这里的key相当于对ip地址和用户名进行拼接
		//之所以用name和ip拼接是因为，仅仅使用ip可能会出现重复ip的情况
		//如果N个客户端处在同一个局域网中，那么服务器端看到的ip就是相同的。
		//NAT机制:数据传输过程中元IP与路由器ip替换，出口路由器的ip地址
		std::stringstream ss;
		// niuxiaoke:123456
		ss<<name<<":"<<addr.sin_addr.s_addr;//uint32
		std::string key; //端口号+ip地址，
		//sockaddr_in value = addr;
		online_friend_list_[ss.str()] = addr;
		return ;
		//已经存在，无副作用  []的方法。
		//不存在就插入，存在就修改,两个[]de 实现
		//ValueTyep& operator[](const KeyType& key) ，采用第一个
		//const ValueTyep& operator[](const KeyType& key) const
		//
		//insert 的返回值类型，std::pair(iterator,bool);
		//
	}
	void ChatServer::DelUser(sockaddr_in addr,const std::string& name)
	{
		//什么样的格式插入，什么样的格式
		std::stringstream ss;
		ss<< name << ":" <<addr.sin_addr.s_addr;
		online_friend_list_.erase(ss.str());
		//若没有key，先find返回迭代器，在删除迭代器
		return ;
	}
	
	void ChatServer::SendMsg(const std::string& data,sockaddr_in addr)
	{
		//1.先要将data序列化
		//
		//2.把这个数据通过 sendto 发送给客户端
		//sendto(sock_,data.c_str());
		//sendto(sock_,data.data(),data.size(),0,(sockaddr*)&addr,sizeof(addr));
		sendto(sock_,data.c_str(),data.size(),0,(sockaddr*)&addr,sizeof(addr));
		LOG(INFO)<<"[Response]"<<inet_ntoa(addr.sin_addr)<<":"<<ntohs(addr.sin_port)<<" "<<data<<"\n";
		//data返回的类型
	}
	
}//end server

//关于一些同学，喜欢使用 using namespace std；不建议。using std：：string
