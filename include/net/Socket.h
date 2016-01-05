#ifndef SOCKET_H__
#define SOCKET_H__

#include "net/SocketApiWrapper.h"
#include "net/InternetAddress.h"
#include "base/Noncopyable.h"

namespace Net
{
	class Socket : private Noncopyable
	{
	public:
		explicit Socket(socket_t sockfd);

		void BindAddress(const InternetAddress& localAddress);
		void Listen();
		socket_t Accept(InternetAddress& peerAddress);

		socket_t GetFd() const { return socketfd_; };

		void ShutdownWrite() { SocketsApi::ShutdownWrite(socketfd_); };

		///
		/// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
		///
		void SetTcpNoDelay(bool on);

		///
		/// Enable/disable SO_REUSEADDR
		///
		void SetReuseAddr(bool on);

		///
		/// Enable/disable SO_REUSEPORT
		///
		void SetReusePort(bool on);

		///
		/// Enable/disable SO_KEEPALIVE
		///
		void SetKeepAlive(bool on);


	private:
		const socket_t socketfd_;
	};
}





#endif


