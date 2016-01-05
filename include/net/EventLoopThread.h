#ifndef EVENT_LOOP_THREAD_H__
#define EVENT_LOOP_THREAD_H__

#include "base/Thread.h"
#include <mutex>
#include <condition_variable>


namespace Net
{
	class EventLoop;
	class EventLoopThread : public Thread
	{
	public:
		EventLoopThread();

		~EventLoopThread();

		EventLoop* StartLoopThread();

	private:
		virtual void Run();


	private:
		EventLoop* loopOfThisThread_;
		std::mutex mutex_;
		std::condition_variable loopCond_;
		
	};
}



#endif