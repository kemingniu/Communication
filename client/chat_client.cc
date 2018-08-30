//chat_client.cc


#include "chat_client.h"



namespace client{
	int ChatClient::Init(const std::string& server_ip,short server_port)
	{
		sock_ = socket(AF_INET,SOCK_DGRAM,0);
		if(sock_ < 0)
		{
			perror("socket");
			return -1;
		}
		
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
	
	
	void ChatClient::SendMsg(const std::string& msg)
	{
		
		server::Data data;
		data.name = name_;
		data.school = school_;
		data.msg = msg;
		//下线
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


