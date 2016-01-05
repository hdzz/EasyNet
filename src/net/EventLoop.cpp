#include "net/EventLoop.h"
#include "net/PollerFactory.h"
#include "net/Channel.h"
#include "base/ThreadLocal.h"
#include "base/PlatformDefine.h"
#include <iostream>
#include <assert.h>




namespace Net
{
	TLS EventLoop* t_threadLocalEventLoop = nullptr;

	EventLoop::EventLoop()
		: poller_(PollerFactory::CreateDefaultPoller(this))
		, eventLoopOwnThreadID_(std::this_thread::get_id())
		, currentActiveChannel_(nullptr)
		, eventHandlingFlag_(false)
	{
		if (t_threadLocalEventLoop)
		{
			std::cout << "Another EventLoop:" << t_threadLocalEventLoop 
				<< "in this thread " << std::this_thread::get_id() << std::endl;
		}
		else
		{
			t_threadLocalEventLoop = this;
		}
	}

	void EventLoop::Loop()
	{
		AssertInLoopThreadOrDie();
		while ( true )
		{
			activeChannelList_.clear();
			poller_->Poll(3000, &activeChannelList_);
			PrintActiveChannels();
			eventHandlingFlag_ = true;
			auto it = activeChannelList_.begin();
			for (; it != activeChannelList_.end(); it++)
			{
				currentActiveChannel_ = *it;
				currentActiveChannel_->HandleEvent();
			}
			currentActiveChannel_ = nullptr;
			eventHandlingFlag_ = false;
			
		} 
	}


	void EventLoop::UpdateChannel(Channel* channel)
	{
		poller_->UpdateChannel(channel);
	}

	void EventLoop::RemoveChannel(Channel* channel)
	{
		assert(channel->GetOwnerLoop() == this);
		AssertInLoopThreadOrDie();
		//if (eventHandling_)
		{
			assert(currentActiveChannel_ == channel ||
				std::find(activeChannelList_.begin(), activeChannelList_.end(), channel) == activeChannelList_.end());
		}
		poller_->RemoveChannel(channel);
	}

	void EventLoop::RunInLoop(const FunctorType& fun)
	{
		AssertInLoopThreadOrDie();
		fun();
	}

	void EventLoop::PushAsyncTaskInLoop(const FunctorType& cb)
	{
		{
			std::unique_lock<std::mutex> lock(mutex_);
			pendingFunctors_.push_back(cb);
		}

		if (!IsInEventLoopOwnThread() || !callingPendingFunctorsNow_)
		{
			//wakeup();
		}
	}

	void EventLoop::AssertInLoopThreadOrDie()
	{
		assert(IsInEventLoopOwnThread());  //In Release version, LOGFATAL
	}

	bool EventLoop::IsInEventLoopOwnThread()
	{
		return eventLoopOwnThreadID_ == std::this_thread::get_id();
	}

	void EventLoop::PrintActiveChannels()
	{
		ChannelList::const_iterator it = activeChannelList_.begin();
		for (it; it != activeChannelList_.end(); it++)
		{
			//for debug
			//std::cout << "ActiveChannels####{ " <<  "Channel:" << *it 
			//	<< " , Event:" << (*it)->RedayEventsToString().c_str()
			//	<< " }" << std::endl;
		}
	}
	
}