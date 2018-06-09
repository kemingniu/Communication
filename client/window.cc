#include "window.h"
#include "../common/util.hpp"
#include<locale.h>

namespace client{
	
	Window::Window()
	{
		//设置字符编码
		setlocale(LC_ALL,"");   // 另外需要在编译时连接 -lncursesw 代替 -lncurses
		//对整个ncurses进行初始化
		initscr();
		//隐藏光标
		curs_set(0);
	}
	Window::~Window()
	{
		//对整个ncurses进行销毁动作
		endwin();
	}
	
	//绘制标题子窗口
	void Window::DrawHeader()
	{
		//LINES是ncueses提供的宏，表示当前窗口的最大行数
		int h = LINES / 5;
		//COLS是ncurses提供的宏，表示当前窗口的最大列数
		int w = COLS;
		int y = 0;
		int x = 0;
		header_win_ = newwin(h,w,y,x);
		std::string title = "欢迎来到聊天室";
		PutStrToWin(header_win_,h/2,w/2-title.size()/2,title);   //先算整体宽度的一般，在减去字符串长度的一半。
		box(header_win_,'|','-');
		wrefresh(header_win_);   //
	}
	//绘制输入框
	void Window::DrawInput()
	{
		//LINES是ncueses提供的宏，表示当前窗口的最大行数
		int h = LINES / 5;
		//COLS是ncurses提供的宏，表示当前窗口的最大列数
		int w = COLS;
		int y = LINES*4/5;
		int x = 0;
		input_win_ = newwin(h,w,y,x);
		std::string title = "亲输入消息";
		PutStrToWin(input_win_,1,2,title);   //先算整体宽度的一般，在减去字符串长度的一半。
		box(input_win_,'|','-');
		wrefresh(input_win_);   //
	}
	//绘制输出框
	void Window::DrawOutput()
	{
		int h = LINES*3/5;
		int w = COLS*3/4;
		int y = LINES/5;
		int x = 0;
		output_win_ = newwin(h,w,y,x);
		box(output_win_,'|','-');
		for(size_t i = 0;i< msgs_.size();i++)
		{
			PutStrToWin(output_win_,i+1,2,msgs_[i]);
		}
		
		wrefresh(output_win_);
	}
	//绘制好友列表
	void Window::DrawFriendList()
	{
		int h = LINES*3/5;
		int w = COLS / 4;
		int y = LINES / 5;
		int x = COLS*3/4;
		
		friend_list_win_ = newwin(h,w,y,x);
		box(friend_list_win_,'|','-');
		//遍历好友列表，显示好友信息
		size_t i = 0;
		for(auto item : friend_list_)
		{
			PutStrToWin(friend_list_win_,i+1,1,item);
			++i;
		}
		wrefresh(friend_list_win_);
	}
	//添加好友信息
	void Window::AddFriend(const std::string& friend_info)
	{
		friend_list_.insert(friend_info);
	}
	//删除好友
	void Window::DelFriend(const std::string& friend_info)
	{
		friend_list_.erase(friend_info);
	}
	
	//往窗口中写字符串
	void Window::PutStrToWin(WINDOW* win,int y,int x,const std::string& str)
	{
		mvwaddstr(win,y,x,str.c_str());
	}
	
	//从窗口中读字符串
	void Window::GetStrFromWin(WINDOW* win,std::string* str)
	{
		char buf[1024*10] = {0};   //这个会造成瓶颈
		wgetnstr(win, buf, sizeof(buf)-1);
		*str = buf;   //string的复制操作时深拷贝。但是有些编译器会优化进行写实拷贝
		//
	}

	
	void Window::AddMsg(const std::string& msg)
	{
		//由于显示的消息条数有限，不能无止境的插入
		//msgs包含的消息数目超过一定的阈值，就把旧消息删除掉.
		//不能随便删，需要持久化的保存在 1.文件中（什么样的格式组织文件内容的格式 json），   2.数据库也可以实现这个，用c语言操纵mysql，官网document，C API。
		// connect  disconnect  mysql也是相当于一个服务器， 发送sql语句，拼装sql语句
		msgs_.push_back(msg);              //超过一定的长度，就换行，
		int max_line = LINES*3/5-2;
		if(max_line < 3)
		{
		LOG(ERROR)<<"MAX_LINE TOO SMALL\n";
		exit(0);
		}

		if(msgs_.size() > max_line)
		{
			msgs_.pop_front();
		}
	}
}//end of client


#ifdef TEST_WINDOW

#include<unistd.h>
int main()
{
	client::Window win;
	win.DrawHeader();
	win.DrawInput();
	win.AddMsg("vdfvgrsbsrbtb"); //构造几条数据
	win.DrawOutput();
	win.AddFriend("afafa|afareg");
	win.DrawFriendList();
	sleep(5);
	return  0;
}
#endif
/*
Makefile

.PHONY:clean
all:test_win test_client

test_win:window.cc
	g++ $^ -o $@ -lpthread -lncursesw -std=c++11



test_client:chat_client.cc
	g++ $^ -o $@ -lpthread -ljsoncpp -lboost_filesystem -lboost_system
.PHONY:clean
clean:
	rm test_client test_win
	
	
	
添加高级字符：
字体管理器：
fonts
查找字符，
	
*/














