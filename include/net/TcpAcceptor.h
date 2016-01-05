#ifndef TCP_ACCEPTOR_H__
#define TCP_ACCEPTOR_H__
#include <memory>
#include <functional>
#include <atomic>
#include "net/InternetAddress.h"
#include "net/Socket.h"
#include "net/Channel.h"



namespace Net
{
	class TcpStream;
	class EventLoop;
	class Channel;
	class TcpAcceptor
	{
		using NewConnectionCallback = std::function<void(int sockfd, const InternetAddress&)> ;
	public:
		explicit TcpAcceptor(EventLoop* loop, const InternetAddress& listenAddress);
		void Listen();
		bool IsListening() const { return listening_.load(); }
		void SetNewConnectionCallBack(const NewConnectionCallback& cb) { newConnCallBack_ = cb; }
		
	private:
		void HandleRead();
		
	private:
		const InternetAddress& inetAddr_;
		Socket listenSocket_;
		Channel acceptChannel_;
		NewConnectionCallback newConnCallBack_;
		EventLoop* loop_;
		std::atomic<bool> listening_;
		
	};
}


#endif