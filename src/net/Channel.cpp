#include "net/Channel.h"
#include "net/EventLoop.h"
#include <iostream>

namespace Net
{
	Channel::Channel(EventLoop* loop, socket_t fd)
		: loop_(loop)
		, fd_(fd)
		, intrestEvent_(EventType::kInvalidEvent)
		, readyEvent_(EventType::kInvalidEvent)
	{

	}

	Channel::~Channel()
	{
		std::cout << "Channel dtor..." << std::endl;
	}


	void Channel::SetInterestEvent(EventType intrestEvent)
	{
		intrestEvent_ = intrestEvent;
		Update();
	}

	void Channel::Update()
	{
		loop_->UpdateChannel(this);
	}

	void Channel::Remove()
	{
		assert(intrestEvent_ == EventType::kInvalidEvent);
		loop_->RemoveChannel(this);
	}


	std::string Channel::RedayEventsToString() const
	{
		std::string readyEventStr;
		if (static_cast<int>(readyEvent_)& static_cast<int>(EventType::kErrorEvent))
		{
			readyEventStr.append("kErrorEvent ");
		}
		if (static_cast<int>(readyEvent_)& static_cast<int>(EventType::kReadEvent))
		{
			readyEventStr.append("kReadEvent ");
		}

		if (static_cast<int>(readyEvent_)& static_cast<int>(EventType::kWriteEvent))
		{
			readyEventStr.append("kWriteEvent ");
		}

		return std::move(readyEventStr);
	}


	void Channel::HandleEvent()
	{
		if (readyEvent_ == EventType::kReadEvent)
		{
			readCallBack_();
		}

		if (readyEvent_ == EventType::kWriteEvent)
		{
			writeCallBack_();
		}
	}


}