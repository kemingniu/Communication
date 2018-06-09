//chat_client.cc


#include "chat_client.h"



namespace client{
	int ChatClient::Init(const std::string& server_ip,short server_port)
	{//命令行参数传递，但是不实用，
	//如果改进从配置文件中读取，ip port，怎么弄，启动前，先打开文件，读取解析
	//json
	/*
	{
		ip:"127.0.0.1",
		port:9090,  //自己实现
	}
	*/
		sock_ = socket(AF_INET,SOCK_DGRAM,0);
		if(sock_ < 0)
		{
			perror("socket");
			return -1;
		}
		//C++11中等价
		//c_str (但是在0x中包含\0)
		//data
		server_addr_.sin_family = AF_INET;
		server_addr_.sin_addr.s_addr = inet_addr(server_ip.c_str());
		server_addr_.sin_port = htons(server_port);
		return 0;
	}
	
	int ChatClient::SetUserInfo(const std::string& name,const std::string& school){
	//
	name_ = name;
	school_ = school;
	return 0;
	}
	
	//用户输入hehe，msg=“hehe”，不光光有msg，换要有姓名，学校，等信息，还要序列化
	void ChatClient::SendMsg(const std::string& msg)
	{
		//这是不合理的，
		//客户端和服务器的交互的接口要严格按照服务器提供的api来操作，
		//sendto(sock_,msg.c_str(),msg.size(),0,(sockaddr*)&server_addr_,sizeof(server_addr_));
		server::Data data;
		data.name = name_;
		data.school = school_;
		data.msg = msg;
		//一下输入内容，表是要下线
	if(data.msg == "quit" || data.msg == "exit" || data.msg == "Q")
			data.cmd = "quit";
		
		std::string str;
		data.Serialize(&str);
		
		//
		sendto(sock_,str.c_str(),str.size(),0,(sockaddr*)&server_addr_,sizeof(server_addr_));
		
		return ;
	}
	
	void ChatClient::RecvMsg(server::Data* data)
	{
		char buf[1024*10] = {0}; //10k
		//由于对端的ip端口号就是服务器的ip端口号，所以此处
		//没必要在获取一边对端的ip，其实已经知道了。
		ssize_t read_size = recvfrom(sock_,buf,sizeof(buf)-1,0,NULL,NULL);
		if(read_size < 0)
		{
			perror("recvfrom");
			return ;
		}
		buf[read_size] = '\0';
		data->UnSerialize(buf);
		return ;
	}
	
}//end of client

//########################################################################################
//先实现一个简易的客户端，通过它来测试客户端和服务器代码
//########################################################################################
#ifdef TEST_CHAT_CLIENT

#include<iostream>


void*  Send(void* arg)
{
//	(void)arg;
	
	client::ChatClient* client = reinterpret_cast<client::ChatClient*>(arg);
	//循环发送数据
	while(true)
	{
		std::string str;
		std::cin>>str;
		client->SendMsg(str);
	}
	return NULL;
}

void*  Recv(void* arg)
{
	client::ChatClient* client = reinterpret_cast<client::ChatClient*>(arg);
	//循环接受数据
	while(true)
	{
		server::Data data;
		client->RecvMsg(&data);
		//[niuxiaoke|gongda] hehe
		std::cout<<"["<<data.name<<"|"<<data.school<<"] "<<data.msg<<"\n";
	}
	
	return NULL;
}


int main(int argc,char* argv[])
{
	if(argc != 3)
	{
		printf("Usage:./client [ip][port]");
	}
		
	client::ChatClient client;
	client.Init(argv[1],atoi(argv[2]));
	std::cout<<"输入用户名：";
	std::string name;
	std::cin>>name;
	std::cout<<"输入学校：";
	std::string school;
	std::cin>>school;
	client.SetUserInfo(name,school);
	
	/*
	std::string str;
	std::cin>>str;
	client.SendMsg(str);
	server::Data data;
	client.RecvMsg(&data);
	*/
	//创建两个线程，分别用于发送，接受数据
	
	pthread_t stid,rtid;
	pthread_create(&stid,NULL,Send,&client);
	pthread_create(&rtid,NULL,Recv,&client);
	pthread_join(stid,NULL);
	pthread_join(rtid,NULL);
	return 0;
}

#endif

/**************************************
Makefile

test_client:chat_client.cc 
	g++ $^ -o $@ -lpthread -ljsoncpp -lboost_filesystem -lboost_system
.PHONY:clean
clean:
	rm test_client

*/

/*
启动服务器,chatserver 0 9090
启动客户端，testclient 127.0.0.1 9090

一起测试：push.sh
scp ./server/chat_server tangzhong@114.115.172.211:/home/test/
scp ./client/test_client tangzhong@114.115.172.211:/homr/test/


scp test@114.115.175.211:/home/test/test_client ./
./test_client 114.115.172.211 9090


现在代码有问题：
在广播消息时，有的客户端无法显示消息.
得出：客户端发送数据后，才会显现。
*/







