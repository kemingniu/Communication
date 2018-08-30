#include "window.h"
#include "../common/util.hpp"
#include<locale.h>

namespace client{
	
	Window::Window()
	{
		
		setlocale(LC_ALL,"");   
		
		initscr();
		
		curs_set(0);
	}
	Window::~Window()
	{
		
		endwin();
	}
	
	//绘制标题子窗口
	void Window::DrawHeader()
	{
		
		int h = LINES / 5;
		
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
		
		int h = LINES / 5;
		
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
		char buf[1024*10] = {0};  
		wgetnstr(win, buf, sizeof(buf)-1);
		*str = buf;   
	}

	
	void Window::AddMsg(const std::string& msg)
	{
		
		
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
