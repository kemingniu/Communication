//chat_server_main.cc

#include "chat_server.h"
#include "../common/util.hpp"


int main(int argc,char* argv[])
{
    if(argc != 3)
	{
		LOG(ERROR)<<"Usage:./chat_server [ip][port]\n";
		return 1;
	}
	server::ChatServer server;
	server.Start(argv[1],atoi(argv[2]));
	
	return 0;
}

/*
main函数简单，可以编译成动态库或者静态库

netstat -anp | grep 9090
*/
