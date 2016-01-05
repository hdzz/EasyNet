#ifndef TCP_CONNECTOR_H__
#define TCP_CONNECTOR_H__

#include <memory>
#include "net/TcpStream.h"


namespace Net
{
	class TcpConnector
	{
	public:
		TcpConnector(InternetAddress inetAddress);
		std::shared_ptr<TcpStream> Connect();
		

	private:
		InternetAddress inetAddress_;
	};
}



#endif