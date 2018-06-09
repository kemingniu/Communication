//在客户端目录下创建一个 window.h
//实现窗口的核心类

#pragma once

#include<string>
#include<deque>
#include<unordered_set>
#include<ncurses.h>
//#include<ncursesw/curses.h>
//#include<locale.h>

namespace client{


class Window{

public:
	Window();//初始哈
	~Window();
	
	//绘制标题子窗口
	void DrawHeader();
	//绘制输入框
	void DrawInput();
	//绘制输出框
	void DrawOutput();
	//绘制好友列表
	void DrawFriendList();
	//往窗口中写字符串
	void PutStrToWin(WINDOW* win,int y,int x,const std::string& str);
	//从窗口中读字符串
	void GetStrFromWin(WINDOW* win,std::string* str);
	//添加消息
	void AddMsg(const std::string& msg);
	//添加好友信息
	void AddFriend(const std::string& friend_info);
	//删除好友
	void DelFriend(const std::string& friend_info);

	WINDOW* header_win_;
	WINDOW* input_win_;
	WINDOW* output_win_;
	WINDOW* friend_list_win_;
private:
	std::deque<std::string> msgs_;  //先进先出，显示的内容有限
	std::unordered_set<std::string> friend_list_;         //只需要显示名字和学校就可以。

};

}//end of client

/*
在插入数据的时候，可以调用数据库，
*/
