//api.hpp

#pragma once
#include<string>
#include<jsoncpp/json/json.h>  /*实用*/

namespace server{


//此处的data相当于服务器给客户端提供的api
struct Data{
	std::string name;
	std::string school;
	std::string msg;
	//对于当前情况下，cmd取值要么为空字符串，要么为quit，如果quit表示客户端要退出
	std::string cmd;
	
	//
	void Serialize(std::string* output){
		Json::Value value;   
		value["name"]   = name;
		value["school"] = school;
		value["msg"]    = msg;
		value["cmd"]    = cmd;
		
		Json::FastWriter write; 
		*output = write.write(value);            
		return ;
	}
	
	void UnSerialize(const std::string& input) //反序列化
	{
		Json::Value value;
		Json::Reader reader;
		reader.parse(input,value);//
		//bool parse(const std::string& document,Value& root,bool collectComments = true);
		//如果不传，就会收集注释
		if(value["name"].isString())//此时如果类型不匹配，会出现异常
		{//isString isInt来判定Json的对象
			name = value["name"].asString();
		}
		school = value["school"].asString();
		msg = value["msg"].asString();
		cmd = value["cmd"].asString();
		return ;
	}
};
}//end of server 
