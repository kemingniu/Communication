#pragma once 
#include<iostream>


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


