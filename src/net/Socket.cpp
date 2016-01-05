#include <iostream>
#include "net/Socket.h"
#include <assert.h>

namespace Net
{

	Socket::Socket(socket_t sockfd) : socketfd_(sockfd)
	{

	}

	void Socket::BindAddress(const InternetAddress& localAddress)
	{
		SocketsApi::Bind(socketfd_, localAddress.GetInetAddress());
	}

	void Socket::Listen()
	{
		SocketsApi::Listen(socketfd_);
	}

	socket_t Socket::Accept(InternetAddress& peerAddress)
	{
	    sockaddr_in_t addr;
		memset(&addr, 0, sizeof addr);
		socket_t connfd = SocketsApi::Accept(socketfd_, &addr);
		assert(connfd > 0);
		peerAddress.SetSockAddrInet(addr);
		return connfd;
	}

	void Socket::SetTcpNoDelay(bool on)
	{
		int optval = on ? 1 : 0;
		::setsockopt(socketfd_, IPPROTO_TCP, TCP_NODELAY,
			reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
	}

	void Socket::SetReuseAddr(bool on)
	{
		int optval = on ? 1 : 0;
		::setsockopt(socketfd_, SOL_SOCKET, SO_REUSEADDR,
			reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
	}

	void Socket::SetReusePort(bool on)
	{
#ifdef SO_REUSEPORT
		int optval = on ? 1 : 0;
		int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
			&optval, static_cast<socklen_t>(sizeof optval));
		if (ret < 0)
		{
			LOG_SYSERR << "SO_REUSEPORT failed.";
		}
#else
		if (on)
		{
			//LOG_EERROR << "SO_REUSEPORT is not supported.";
			std::cout << "SO_REUSEPORT is not supported." << std::endl;
		}
#endif
	}

	void Socket::SetKeepAlive(bool on)
	{
		int optval = on ? 1 : 0;
		::setsockopt(socketfd_, SOL_SOCKET, SO_KEEPALIVE,
			reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
	}
}