#include "net/InternetAddress.h"
#include "net/Endian.h"
#include "net/SocketApiWrapper.h"
#include <assert.h>


namespace Net
{
	InternetAddress::InternetAddress(port_t port)
	{
		memset(&socketAddrIn_, 0, sizeof socketAddrIn_);
		socketAddrIn_.sin_family = AF_INET;
		in_addr_t ip;
		ip.S_un.S_addr = INADDR_ANY;
		socketAddrIn_.sin_addr.s_addr = Endian::HostToNetwork32(ip.S_un.S_addr);
		socketAddrIn_.sin_addr.s_addr = htonl(ip.S_un.S_addr);
		socketAddrIn_.sin_port = Endian::HostToNetwork16(port);
	}

	InternetAddress::InternetAddress(const std::string& ip, uint16_t port)
	{
		memset(&socketAddrIn_, 0, sizeof socketAddrIn_);
		SocketsApi::FromIpPort(ip.c_str(), port, &socketAddrIn_);
	}

	InternetAddress::InternetAddress(sockaddr_in_t& addr) : socketAddrIn_(addr)
	{
		
	}

	std::string InternetAddress::GetIp() const
	{
		char buf[32] = { '\0' };
		SocketsApi::ToIp(buf, sizeof buf, socketAddrIn_);
		return buf;
	}

	std::string InternetAddress::GetIpAndPort() const
	{
		char buf[32];
		SocketsApi::ToIpPort(buf, sizeof buf, socketAddrIn_);
		return buf;
	}


}