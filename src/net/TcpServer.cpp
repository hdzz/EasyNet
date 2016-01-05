#include <iostream>
#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "net/TcpAcceptor.h"
#include "net/TcpConnection.h"
#include "net/Socket.h"
#include "net/SocketApiWrapper.h"
#include <sstream>



namespace Net
{
	TcpServer::TcpServer(EventLoop* loop,
		const InternetAddress& listenAddr,
		std::string& serverName = std::string("Unknown"))
		: loop_(loop)
		, threadGroup_(new EventLoopThreadGroup(loop_))
		, acceptor_(new TcpAcceptor(loop, listenAddr))
		, nextConnId_(1)
		, hostIpPort_(listenAddr.GetIpAndPort())
		, name_(serverName)
		, connCallBack_(DefaultConnectionCallback)
	{
		acceptor_->SetNewConnectionCallBack(
			std::bind(&TcpServer::NewConnection, 
			this, 
			std::placeholders::_1, 
			std::placeholders::_2));
	}

	void TcpServer::Start()
	{
		threadGroup_->Start();
		assert(!acceptor_->IsListening());
		acceptor_->Listen();
	}

	void TcpServer::NewConnection(socket_t fd, const InternetAddress& peerAddr)
	{
		loop_->AssertInLoopThreadOrDie();
		EventLoop* ioLoop = threadGroup_->GetNextLoop();

		InternetAddress localAddr(SocketsApi::GetLocalAddr(fd));
		std::stringstream connNameSs;
		connNameSs << name_ << hostIpPort_ << "#" << nextConnId_;
		nextConnId_++;
		std::string connName = connNameSs.str();

		std::cout << "[" << name_
			<< "] - new connection [" << connName
			<< "] from " << peerAddr.GetIpAndPort()
			<< std::endl;
		TcpConnectionPtr conn(new TcpConnection(loop_, connName, fd, localAddr, peerAddr));
		//Save the new TcpConnection to this map, because life cycle is manage by shared_ptr
		allConnections_[connNameSs.str()] = conn;
		conn->SetConnectionCallBack(connCallBack_);
		conn->SetMessageCallBack(msgCallBack_);
		conn->SetCloseCallBack(std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
		ioLoop->RunInLoop(std::bind(&TcpConnection::ConnectionEstablished, conn));
	}

	void TcpServer::RemoveConnection(const TcpConnectionPtr& conn)
	{
		assert(allConnections_.find(conn->GetConnectionName()) != allConnections_.end());
		allConnections_.erase(conn->GetConnectionName());
		conn->ConnectionDestroyed();
	}

}