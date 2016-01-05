#ifndef TCP_STREAM_H__
#define TCP_STREAM_H__
#include "net/InternetAddress.h"
#include "base/Noncopyable.h"
#include <string>

namespace Net
{
	class TcpStream	: Noncopyable
	{
	public:
		TcpStream(socket_t sockFd, InternetAddress& inetAddress);
		size_t Send(const char* buffer, size_t len);
		size_t Receive(char* buffer, size_t len, uint32_t iTimeoutSeconds);
		std::string GetPeerIP() const;
		port_t GetPeerPort() const;
		

	private:
		
		TcpStream();
		TcpStream(const TcpStream& rhs);
		TcpStream(const TcpStream&& rhs);
		bool WaitForSocketReadEvent(uint32_t iTimeoutSeconds);

	private:
		socket_t sockFd_;
	};

}



#endif