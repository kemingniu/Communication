#pragma once 
#include<iostream>
#include<vector>
#include<boost/algorithm/string.hpp>
#include<sys/time.h>
#include<sys/socket.h>
#include<fstream>
#include<boost/filesystem.hpp>
#include<unordered_map>


class TimeUtil
{
public:

	//获取当前的妙级
		static int64_t TimeStamp()    //时间在不断地扩大，所以采用大整形的数据来存储时间差
		{
		struct timeval tv;
		gettimeofday(&tv,NULL);
		return tv.tv_sec;
		}
		
		//获取当前的微妙级
		static int64_t TimeStampUS()  
		{
		struct timeval tv;
		gettimeofday(&tv,NULL);
		return 1000*1000*tv.tv_sec+tv.tv_usec;  //微秒级时间戳，距离1970年1月1日0时0分0秒
		}
};


enum LogLevel    //日志等级
{
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	CRITIAL,
};

//LOG(INFO)<<"hello";
//[I12344566 util.hpp 234]
inline std::ostream& Log(LogLevel level,const char* file,int line)  //熟悉<< >>的友元重写
{
	std::string prefix = "I";
	if(level == WARNING)
	{
		prefix = "W";
	}
	else if(level == ERROR)
	{
		prefix = "E";
	}
	else if(level == CRITIAL)
	{
	prefix = "C";
	}
	else if(level == DEBUG)
	{
		prefix = "D";
	}
	
	//TimeUtil t;  如果不想创建对象，那就建成静态，
	
	std::cout<<"["<<prefix<<TimeUtil::TimeStampUS()<<" "<<file<<":"<<line<<"]";
	
	return std::cout; //返回cout为了可以拼接
}

//当前 文件名，行号
//__FILE__   __LINE__

#define LOG(level) Log(level,__FILE__,__LINE__)//字符串替换

//代码大全 code complete

class FileUtil
{
public:
		//从文件描述符中读取一行
		//一行的界定标识 \n \r  \r\n
		//返回的inline对象中是不包含界定标识符的
		//aaa\nbbb\nccc
		//返回后是aaa
		static int ReadLine(int fd,std::string* line)
		{
			line->clear();
			/*
			{
				string tmp;
				str.swap(tmp);  内存泄漏
			}
			*/
			while(true)
			{
				char c = '\0';
				ssize_t read_size = recv(fd,&c,1,0);//
				if(read_size <= 0)
				{
					perror("recv");
					return -1;
				}
				//如果当前字符是\r,把这个情况处理厂成\n
				if(c == '\r')
				{
					//MSG_PEEK虽然从缓冲区读了一个字符，但是缓冲区并没有把他删掉
				recv(fd,&c,1,MSG_PEEK);
				if(c == '\n')
				{
					//发现\R后面的字符就是\n，为了不影响下次循环，
					//需要把这样的字符从缓冲区中删掉
					recv(fd,&c,1,0);
					
				}
				else{   //一行的界定标识 \n \r  \r\n
					c = '\n';
				}
				}
				//这个条件包含了\r \r\n
				if(c == '\n')
				{
					break;
				}
				line->push_back(c);
			}
			return 0;
		}
		
		
	static int ReadN(int fd,size_t len,std::string* output)
	{
		output->clear();
		char c = '\0';
		for(size_t i = 0;i < len;i++)
		{
			recv(fd,&c,1,0);
			output->push_back(c);  //默认阻塞 content_length = size，
		}
		return 0;
	}
	
	//从文件中读取全部内容到到是std::string中。
	//sendfile   内核中直接数据传输，避免内核和用户态的交替
	static int ReadAll(const std::string& file_path,std::string* output)
{
		std::ifstream file(file_path.c_str()); //文件的读写
		if(!file.is_open())
	{
		LOG(ERROR)<<"OPEEN ERROR! FILE_PATH:"<<file_path<<"\n";
		return -1;
	}
		file.seekg(0,file.end);//调整文件指针的位置 ，此处文件末尾
		int length = file.tellg();//查询当前文件的指针位置，返回文件指针位置相对于其实位置的偏移量
		//2G，
		file.seekg(0,file.beg);//为了从头读取文件。，需要把文件指针设置到开头位置
	
		output->resize(length);
		file.read(const_cast<char*>(output->c_str()),length);//读取完整的文件内容
	
		//忘记写close，在对象销毁时，析构会自动的销毁文件描述符。
		file.close();
		return 0;
}

	static int ReadAll(int fd,std::string* output)
	{
		//实现从文件描述符读取所有数据
		while(true)
		{
			char buf[1024] = {0};
			ssize_t read_size = read(fd,buf,sizeof(buf)-1);
			if(read_size < 0)
			{
				perror("read");
				return -1;
			}
			if(read_size ==0)
			{
				return 0;
			}
			buf[read_size] = '\0';
			(*output) += buf;
		}
		return 0;
	}
	static bool IsDir(const std::string& file_path)
	{
		return boost::filesystem::is_directory(file_path);
	}
};



class StringUtil
{
	public:
	     //把一个字符串安装split_char进行切分，分成的N的子串
			static int Split(const std::string& input,const std::string& split_char,
			std::vector<std::string>* output)
			{
				//token_compress_on 
				//例如分隔符是空格，字符串时“a  b”   l两个空格连载一起
				//对于这个情况，返回的结构就是"a" "b"
				//token_compress_off 就是"a" "" "b"
				boost::split(*output,input,boost::is_any_of(split_char),boost::token_compress_on);
				return 0;
			}
			
			typedef std::unordered_map<std::string,std::string> UrlParam;
			static int ParseUrlParam(const std::string& input,UrlParam* output)
			{
				//1.先按取地址符号切分成如干个 kv
				std::vector<std::string> params;
				Split(input,"&",&params);
				//在针对每一个kv，按=切分，地方到输入结果
				for(auto it : params)
				{
					std::vector<std::string> kv;
					Split(it,"=",&kv);
					if(kv.size() !=2)
					{
						LOG(WARNING)<<"kv format eror! item="<<it<<"\n";
						continue;
					}
					(*output)[kv[0]] = kv[1];
				}
				return 0;
			}
};

