/**
 * 阻塞式的队列
 * 获得队列中元素时，如果队列为空会阻塞等待
 * 如果队列有最大容量限制，在放入队列数据时如果队列中最大元素已满会阻塞等待
 */

#pragma once
#include <condition_variable>
#include <mutex>
#include <deque>

// 模板类
template<class T>
class BlockQueue{
public:
	typedef std::unique_lock<std::mutex> TLock;
    typedef T type;
 
	//maxCapacity为-1，代表队列无最大限制
	explicit BlockQueue(const int maxCapacity = -1):m_maxCapacity(maxCapacity){
	}
 
	size_t size(){
		TLock lock(m_mutex);
		return m_list.size();
	}
 
	void push_back(const type &item){
		TLock lock(m_mutex);
		if (true == hasCapacity()){
			while (m_list.size() >= m_maxCapacity){
                // 阻塞，等待队列有空的位置
				m_notFull.wait(lock);
			}
		}
		m_list.push_back(item);
		m_notEmpty.notify_all();
	}
 
	type pop(){
		TLock lock(m_mutex);
		while (m_list.empty()){
            // 阻塞，等待队列中有 元素
			m_notEmpty.wait(lock);
		}
 
		type temp = *m_list.begin();
		m_list.pop_front();

		// 使用 notify_one() 可能效率更高一些，但是 notify_all() 更安全
		m_notFull.notify_all();
 
		lock.unlock();
		return temp;
	}
 
	bool empty(){
		TLock lock(m_mutex);
		return m_list.empty();
	}
 
	bool full(){
		if (false == hasCapacity()){
			return false;
		}
		TLock lock(m_mutex);
		return m_list.size() >= m_maxCapacity;
	}
 
private:
	// block queue 是否有 最大容量限制
	bool hasCapacity() const{
		return m_maxCapacity > 0;
	}
 
	typedef std::deque<type> TList;
	TList m_list;
 
	const int m_maxCapacity;
 
	std::mutex m_mutex;
	std::condition_variable m_notEmpty;
	std::condition_variable m_notFull;
};
