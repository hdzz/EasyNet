#ifndef EVENT_LOOP_H__
#define EVENT_LOOP_H__

#include "base/Noncopyable.h"
#include <memory>
#include <functional>
#include <list>
#include <net/Poller.h>
#include <thread>
#include <mutex>
#include <atomic>

class Poller;
namespace Net
{
	class Channel;
	class SelectPoller;
	using FunctorType = std::function<void()>;
	

	class EventLoop : private Noncopyable
	{
	public:
		EventLoop();

		void Loop();

		void UpdateChannel(Channel* channel);

		void RemoveChannel(Channel* channel);

		void RunInLoop(const FunctorType& fun);

		void PushAsyncTaskInLoop(const FunctorType& cb);

		void AssertInLoopThreadOrDie();

		bool IsInEventLoopOwnThread();

		void PrintActiveChannels();

		


	private:
		using ChannelList = std::list<Channel*>;
		ChannelList activeChannelList_;
		Channel* currentActiveChannel_;
		std::atomic<bool> eventHandlingFlag_;
		std::unique_ptr<Poller> poller_;
		std::thread::id eventLoopOwnThreadID_;
		std::mutex mutex_;
		std::list<FunctorType> pendingFunctors_;
		std::atomic<bool> callingPendingFunctorsNow_;
	};
}



#endif