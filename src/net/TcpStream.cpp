#include "net/TcpStream.h"


namespace Net
{
	TcpStream::TcpStream(socket_t sockFd, InternetAddress& inetAddress)
		:sockFd_(sockFd)
	{

	}

	size_t TcpStream::Send(const char* buffer, size_t len)
	{
		return SocketsApi::Send(sockFd_, buffer, len);
	}

	size_t TcpStream::Receive(char* buffer, size_t len, uint32_t iTimeoutSeconds)
	{
		if (!iTimeoutSeconds)
		{
			return SocketsApi::Receive(sockFd_, buffer, len);
		}

		if (WaitForSocketReadEvent(iTimeoutSeconds))
		{
			return SocketsApi::Receive(sockFd_, buffer, len);
		}
		return 0;
	}

	

	bool TcpStream::WaitForSocketReadEvent(uint32_t iTimeoutSeconds)
	{
		fd_set sock_read_sets;
		struct timeval tv;
		tv.tv_sec = iTimeoutSeconds;
		tv.tv_usec = 0;
		FD_ZERO(&sock_read_sets);
		FD_SET(sockFd_, &sock_read_sets);
		return 	select(sockFd_ + 1, &sock_read_sets, NULL, NULL, &tv) > 0;
	}

	//std::string TcpStream::GetPeerIP() const
	//{

	//}

	//port_t TcpStream::GetPeerPort() const
	//{

	//}
}