#ifndef CHANNEL_H__
#define CHANNEL_H__

#include "net/SoketDefine.h"
#include "base/Noncopyable.h"
#include <functional>

namespace Net
{
	class EventLoop;
	enum class EventType : unsigned int
	{ 
		kInvalidEvent	= 0x00, 
		kReadEvent		= 0x01, 
		kWriteEvent		= 0x02, 
		kErrorEvent		= 0x04
	};

	class Channel final : private Noncopyable
	{
	public:
		using EventCallBack = std::function<void(void)>;

		Channel(EventLoop* loop, socket_t fd);

		~Channel();

		void HandleEvent(void);

		void SetInterestEvent(EventType intrestEvent);

		void SetReadyEvent(EventType event) { readyEvent_ = event; }

		void DisableAll() { intrestEvent_ = EventType::kInvalidEvent; }

		void DisableWriting() 
		{ 
			intrestEvent_ = static_cast<EventType>(static_cast<int>(intrestEvent_) &
				~static_cast<int>(EventType::kWriteEvent));
			Update(); 
		}

		socket_t GetSocketFd() const { return fd_; }

		EventType GetEventType() const { return intrestEvent_; }

		bool IsWriting()
		{ 
			return static_cast<int>(intrestEvent_)&
				static_cast<int>(EventType::kWriteEvent);
		}

		void Remove();

		EventLoop* GetOwnerLoop() const { return loop_; }
		
		std::string RedayEventsToString() const;

		void SetReadCallBack(EventCallBack cb) { readCallBack_ = cb; }
		void SetWriteCallBack(EventCallBack cb) { writeCallBack_ = cb; }


	private:
		void Update();

	private:
		EventLoop* loop_;
		socket_t fd_;
		EventType intrestEvent_;
		EventType readyEvent_;
		EventCallBack readCallBack_;
		EventCallBack writeCallBack_;
		EventCallBack closeCallBack_;
		EventCallBack errorCallBack_;
	};
}

#endif