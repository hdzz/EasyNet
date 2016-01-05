#ifndef POLLER_H__
#define POLLER_H__

#include <chrono>
#include <list>

namespace Net
{
	class Channel;
	class EventLoop;
	class Poller
	{
		
	public:
		using ChannelList = std::list<Channel*>;
		Poller(EventLoop* eventLoop) : eventLoop_(eventLoop)
		{

		}
		virtual ~Poller() {}
		virtual void Poll(int timeoutMs, ChannelList* activeChannels) = 0;
		virtual void UpdateChannel(Channel* channel) = 0;
		virtual void RemoveChannel(Channel* channel) = 0;	 

	private:
		EventLoop* eventLoop_;
	};
}



#endif