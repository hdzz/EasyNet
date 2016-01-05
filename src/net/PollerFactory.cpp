#ifndef POLLER_FACTORY_H__
#define POLLER_FACTORY_H__

#include "base/Singleton.h"
#include "net/PollerFactory.h"
#include "net/SelectPoller.h"

namespace Net
{
	//This poller factory create the default poller base on different platforms//
	Poller* PollerFactory::CreateDefaultPoller(EventLoop* loop)
	{
#ifdef WINDOWS
		return Singleton<Net::SelectPoller>::GetInstance(loop);
#elif LINUX

#endif
		return nullptr;
	}
}


#endif