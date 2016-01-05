#include "net/TcpAcceptor.h"
#include "net/EventLoop.h"
#include "net/Socket.h"
#include <iostream>


namespace Net
{
	TcpAcceptor::TcpAcceptor(EventLoop* loop, const InternetAddress& listenAddress) 
		: inetAddr_(listenAddress)
		, loop_(loop)
		, listenSocket_(SocketsApi::CreateNonBlockingSocket())
		, acceptChannel_(loop, listenSocket_.GetFd())
		, listening_(false)
	{
		listenSocket_.SetReuseAddr(true);
		listenSocket_.SetReusePort(false);
		listenSocket_.BindAddress(listenAddress);
		acceptChannel_.SetReadCallBack(std::bind(&TcpAcceptor::HandleRead, this));
	}
		 
	void TcpAcceptor::Listen()
	{
		listening_.store(true);
		listenSocket_.Listen();
		acceptChannel_.SetInterestEvent(EventType::kReadEvent);
	}



	void TcpAcceptor::HandleRead()
	{
		// listen socket is readable, a new connetion is request
		InternetAddress peerAddress;
		socket_t connfd = listenSocket_.Accept(peerAddress);
		assert(connfd >= 0);
		if (newConnCallBack_)
		{
			newConnCallBack_(connfd, peerAddress);
		}
		else
		{
			// since non handler for the read event, shutdown the connection directly
			SocketsApi::Close(connfd);
		}
	}
}