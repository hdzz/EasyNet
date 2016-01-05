#include "net/TcpConnector.h"



namespace Net
{
	
	TcpConnector::TcpConnector(InternetAddress inetAddress)
		: inetAddress_(inetAddress)
	{

	}

	std::shared_ptr<TcpStream> TcpConnector::Connect()
	{
		socket_t sockfd = SocketsApi::CreateNonBlockingSocket();
		if (SocketsApi::Connect(sockfd, inetAddress_.GetInetAddress()) != 0)
		{
			SocketsApi::Close(sockfd);
			//LOG_SYSFATAL << "Can not connected to the server";
		}
		return std::make_shared<TcpStream>(sockfd, inetAddress_);
	}
}