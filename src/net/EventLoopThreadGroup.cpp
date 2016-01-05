#include "net/EventLoopThreadGroup.h"
#include "net/EventLoopThread.h"
#include "net/EventLoop.h"
#include <assert.h>



namespace Net
{
	EventLoopThreadGroup::EventLoopThreadGroup(EventLoop* baseLoop, int threadNum)
		: baseLoop_(baseLoop)
		, numThreads_(threadNum)
		, startFlag_(false)
		, loopIndex_(0)
	{

	}


	void EventLoopThreadGroup::Start()
	{
		assert(!startFlag_);
		baseLoop_->AssertInLoopThreadOrDie();
		for (auto i = 0; i < numThreads_; i++)
		{
			EventLoopThread* loopThread = new EventLoopThread();
			loopThreads_.push_back(loopThread);
			auto loop = loopThread->StartLoopThread();
			assert(loop);
			loops_.push_back(loop);
		}
		startFlag_ = true;
	}


	EventLoop* EventLoopThreadGroup::GetNextLoop() const
	{
		baseLoop_->AssertInLoopThreadOrDie();
		assert(startFlag_);
		EventLoop* loop = baseLoop_;

		if (!loops_.empty())
		{
			// round-robin
			loop = loops_[loopIndex_];
			loopIndex_++;
			if (loopIndex_ >= loops_.size())
			{
				loopIndex_ = 0;
			}
		}
		return loop;
	}
}