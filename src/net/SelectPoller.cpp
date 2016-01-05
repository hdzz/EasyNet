#include "net/SelectPoller.h"
#include "net/Channel.h"
#include "net/SoketDefine.h"
#include <assert.h>
#include <iostream>


namespace Net
{
	SelectPoller::SelectPoller(EventLoop* loop) : Poller(loop)
	{
		FD_ZERO(&select_readfds_);
		FD_ZERO(&select_writefds_);
		FD_ZERO(&select_expectfds_);
		FD_ZERO(&write_fds_);
		FD_ZERO(&read_fds_);
		FD_ZERO(&expect_fds_);
	}

	void SelectPoller::UpdateChannel(Channel* channel)
	{
		if (polledChannelsMap_.find(channel->GetSocketFd()) == polledChannelsMap_.end())
		{
			polledChannelsMap_.insert(std::pair<socket_t, Channel*>(channel->GetSocketFd(), channel));
			fdSet_.insert(channel->GetSocketFd());
		}
		Update(channel);
	}

	void SelectPoller::RemoveChannel(Channel* channel)
	{
		assert(channel);
		socket_t fd = channel->GetSocketFd();
		assert(polledChannelsMap_.find(fd) != polledChannelsMap_.end());  
		assert(fdSet_.find(fd) != fdSet_.end());
		FD_CLR(fd, &select_readfds_);
		FD_CLR(fd, &select_writefds_);
		FD_CLR(fd, &select_expectfds_);
		polledChannelsMap_.erase(fd);
		fdSet_.erase(fd);
	}

	void SelectPoller::Update(Channel* channel)
	{
		assert(channel);
		if (channel->GetEventType() == EventType::kReadEvent)
		{
			FD_SET(channel->GetSocketFd(), &select_readfds_);
		}
		else
		{
			FD_CLR(channel->GetSocketFd(), &select_readfds_);
		}
		if (channel->GetEventType() == EventType::kWriteEvent)
		{
			FD_SET(channel->GetSocketFd(), &select_writefds_);
		}
		else
		{
			FD_CLR(channel->GetSocketFd(), &select_writefds_);
		}
	}

	void SelectPoller::Poll(int timeoutMs, ChannelList* activeChannels)
	{
		struct timeval tv;
		tv.tv_sec = timeoutMs/1000;
		tv.tv_usec = (timeoutMs % 1000) * 1000;
		read_fds_ = select_readfds_;
		write_fds_ = select_writefds_;
		expect_fds_ = select_expectfds_;
		int maxFd = *fdSet_.begin();
		int numActiveEvents = ::select(maxFd + 1, &read_fds_, &write_fds_, &expect_fds_, &tv);
		if (numActiveEvents > 0)
		{
			FillActiveChannel(numActiveEvents, activeChannels);
		}
		else if (!numActiveEvents)
		{
			//std::cout << "Nothing happend..." << std::endl;
		}
		else
		{
			std::cout << "select system call error...." << std::endl;
		}
	}

	void SelectPoller::FillActiveChannel(int activeEventsNum, ChannelList* channelList) 
	{
		EventType readyEvent = EventType::kInvalidEvent;
		for (auto it = fdSet_.begin(); it != fdSet_.end() && activeEventsNum > 0; it++)
		{
			socket_t fd = *it;
			if (FD_ISSET(fd, &read_fds_)) readyEvent = EventType::kReadEvent;
			if (FD_ISSET(fd, &write_fds_)) readyEvent = EventType::kWriteEvent;

			if (readyEvent != EventType::kInvalidEvent)
			{
				activeEventsNum--;
				Channel* singleActiveChannel = polledChannelsMap_.find(fd)->second;
				assert(singleActiveChannel);
				singleActiveChannel->SetReadyEvent(readyEvent);
				channelList->push_back(singleActiveChannel);
			}
		}
	}
}