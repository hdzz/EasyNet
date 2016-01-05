#ifndef ICALL_BACK_TYPE_H__
#define ICALL_BACK_TYPE_H__
#include <memory>
#include <functional>

namespace Net
{
	class TcpConnection;
	class ChannelBuffer;
	using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
	using TcpConnectionCallback = std::function < void(const TcpConnectionPtr&)>;
	using TcpMessageCallback = std::function<void(const TcpConnectionPtr&, ChannelBuffer*)>;
	using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
	using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;

	using CodecMessageCallback = std::function<void(const TcpConnectionPtr&, const std::string& message)>;

	void DefaultMessageCallback(const TcpConnectionPtr& conn, ChannelBuffer* buffer);
	void DefaultConnectionCallback(const TcpConnectionPtr& conn);
	void DefaultConnectionDestroyCallback(const TcpConnectionPtr& conn);
}

#endif