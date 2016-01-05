#include <iostream>
#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "net/ChannelBuffer.h"
#include "net/LineBasedFrameCodec.h"
#include <functional>



class LogServer
{
public:
	LogServer(Net::EventLoop* loop, Net::InternetAddress& listenAddress)
		: server_(loop, listenAddress, std::string("LogServer"))
		, lineCodec_(std::bind(&LogServer::onMessage, this, std::placeholders::_1, std::placeholders::_2))
	{
		server_.SetConnectionCallBack(
			std::bind(&LogServer::onConnection, this, std::placeholders::_1));
		server_.SetMessageCallBack(
			std::bind(&Net::LineBasedFrameCodec::Decode, &lineCodec_, std::placeholders::_1, std::placeholders::_2));
	}

	void Start()
	{
		server_.Start();
	}

private:
	void onConnection(Net::TcpConnectionPtr conn)
	{
		std::cout << "EchoServer connection callback....." << std::endl;
	}

	void onConnectionDestroy(Net::TcpConnectionPtr conn)
	{
		std::cout << "EchoServer connection Destroy callback...." << std::endl;
	}

	void onMessage(const Net::TcpConnectionPtr& conn, const std::string& message)
	{
		std::cout << "In LogServer, message is:" << message << std::endl;
	}

private:
	Net::TcpServer server_;
	Net::LineBasedFrameCodec lineCodec_;
};



int main()
{
	Net::EventLoop loop;
	Net::InternetAddress serverAddress("10.8.77.37", 8888);
	LogServer server(&loop, serverAddress);
	server.Start();
	loop.Loop();
	std::string result;
	result.resize(4);
	result.append("Hello world.");
	system("pasue");
	return 0;
}

