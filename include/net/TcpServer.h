#ifndef TCP_SERVER_H__
#define TCP_SERVER_H__
#include "net/InternetAddress.h"
#include <functional>
#include <memory>
#include "net/ICallBackDefine.h"
#include "net/TcpConnection.h"
#include "net/EventLoopThreadGroup.h"
#include <map>

namespace Net
{
	class TcpConnection;
	class EventLoop;
	class TcpAcceptor;
	class TcpServer
	{
	public:	  

		TcpServer(EventLoop* loop, const InternetAddress& listenAddr, std::string& serverName);

		void Start();

		void SetConnectionCallBack(const TcpConnectionCallback& cb) { connCallBack_ = cb; }

		void SetMessageCallBack(const TcpMessageCallback& cb) { msgCallBack_ = cb; }

		void NewConnection(socket_t fd, const InternetAddress& peerAddr);

		void RemoveConnection(const TcpConnectionPtr& conn);

	private:
		
		EventLoop* loop_;
		TcpAcceptor* acceptor_;
		std::unique_ptr<Net::EventLoopThreadGroup> threadGroup_;
		TcpConnectionCallback connCallBack_;
		TcpConnectionCallback connDestroyCallBack_;
		TcpMessageCallback msgCallBack_;
		uint64_t nextConnId_;
		std::string hostIpPort_;
		std::string name_;
		std::map<std::string, TcpConnectionPtr> allConnections_;

	};
}


#endif