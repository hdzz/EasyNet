#ifndef EVENT_LOOP_THREAD_GROUP_H__
#define EVENT_LOOP_THREAD_GROUP_H__
#include <stdint.h>

#include <vector>
namespace Net
{
	class EventLoop;
	class EventLoopThread;
	class EventLoopThreadGroup
	{
	public:
		EventLoopThreadGroup(EventLoop* baseLoop, int threadNum = 0);
		void Start();
		EventLoop* GetNextLoop() const;
		void SetThreadNum(int threadNum) { numThreads_ = threadNum; }



	private:
		EventLoop* baseLoop_;
		int numThreads_;
		bool startFlag_;
		mutable uint32_t loopIndex_;
		std::vector<EventLoop*> loops_;
		std::vector<EventLoopThread*> loopThreads_;
		
	};
}


#endif