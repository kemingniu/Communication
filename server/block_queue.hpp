//block_queue.hpp /*阻塞队列，生产者消费者的封装*/

#pragma once
#include<vector>
#include<semaphore.h>


namespace server{

template<typename T>  
class BlockQueue{  
public:

	BlockQueue(size_t s = 1024)
	:data_(s)
	,head_(0)
	,tail_(0)
	,size_(0)
	{  
		sem_init(&sem_data_,0,0);    
     	        sem_init(&sem_blank_,0,s);   
	}
	~BlockQueue()
	{
		sem_destroy(&sem_data_);
		sem_destroy(&sem_blank_);
	}
	
	void Push_Back(const T& value)
	{
		//同步的控制
		sem_wait(&sem_blank_);
		data_[tail_++] = value;       //约定[head_,tail_)
		if(tail_ >= data_.size())
		{
			tail_ = 0;
		}
		++size_;
		sem_post(&sem_data_);
	}
	
	void PopFront(T* value)
	{
		sem_wait(&sem_data_);
		*value = data_[head_++];
		if(head_ > data_.size())
		{
			head_ = 0;
		}
		--size_;
		sem_post(&sem_blank_);
	}

private:	
	std::vector<T> data_;
	sem_t sem_data_;    //元素的个数
	sem_t sem_blank_;   //空闲的个数
	size_t head_;  //指定队首队尾   size大小
	size_t tail_;
	size_t size_;
};
}//end server
