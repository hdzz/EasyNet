#ifndef ASYNC_QUEUE_H__
#define ASYNC_QUEUE_H__
#include <deque>
#include <condition_variable>
#include <mutex>
#include <atomic>


template <class T, uint64_t QUEUE_SIZE = 1000>
class ASyncQueue
{
public:
	ASyncQueue() : queue_()
	{

	}

	void Put(T&& value)
	{
		std::unique_lock<std::mutex> lk(mutex_);
		while (Full())
		{
			notFullCond_.wait(lk);
		}

		queue_.emplace_back(std::move(value));
		notEmptyCond_.notify_one();
	}

	void Put(T value)
	{
		std::unique_lock<std::mutex> lk(mutex_);
		while (Full())
		{
			notFullCond_.wait(lk);
		}
		queue_.emplace_back(value);
		notEmptyCond_.notify_one();
	}

	T Take()
	{
		std::unique_lock<std::mutex> lk(mutex_);
		while (Empty())
		{
			notEmptyCond_.wait(lk);
		}
		T frontValue(std::move(queue_.front()));   //T must have move constructor
		queue_.pop_front();
		notFullCond_.notify_one();
		return frontValue;
	}

	uint64_t Size() const
	{
		return queue_.size();
	}

	bool Empty() const
	{
		bool empty = queue_.empty();
		return empty;
	}

private:
	bool Full()	const
	{
		bool full = (queue_.size() >= QUEUE_SIZE);
		return full;
	}

private:
	mutable std::mutex mutex_;
	std::condition_variable notEmptyCond_;
	std::condition_variable notFullCond_;
	std::deque<T> queue_;
};


#endif