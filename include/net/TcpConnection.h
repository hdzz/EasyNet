#ifndef TCPCONNECTION_H__
#define TCPCONNECTION_H__

#include <string>
#include "net/ICallBackDefine.h"
#include "net/InternetAddress.h"
#include "net/Socket.h"
#include "base/Noncopyable.h"
#include "base/StringPiece.h"
#include "net/Channel.h"
#include "net/ChannelBuffer.h"



namespace Net
{
	class EventLoop;
	class TcpConnection final : private Noncopyable 
							  , public std::enable_shared_from_this<Net::TcpConnection>
	{
	public:
		TcpConnection(EventLoop* loop,
			const std::string& name,
			int sockfd,
			const InternetAddress& localAddr,
			const InternetAddress& peerAddr);

		TcpConnection(const TcpConnection&) = delete;
		TcpConnection& operator= (const TcpConnection&) = delete;

		~TcpConnection();

		void SetConnectionCallBack(const TcpConnectionCallback& cb) { connectionCallback_ = cb; }
		void SetMessageCallBack(const TcpMessageCallback& cb) { messageCallback_ = cb; }
		void SetWriteCompleteCallback(const WriteCompleteCallback& cb){ writeCompleteCallback_ = cb;}
		void SetCloseCallBack(const CloseCallback& cb) { closeCallBack_ = cb; }

		EventLoop* GetConnectionEventLoop() const { return  loop_; }
		std::string GetConnectionName() const { return connectionName_; }
		bool IsConnected() const { return connectionState_ == ConnectionState::kConnected; }

		void ConnectionEstablished();
		void ConnectionDestroyed();

		const InternetAddress& GetLocalAddress() const { return localAddress_; }
		const InternetAddress& GetPeerAddress() const { return peerAddress_; }

		void Send(const void* message, int len);
		void Send(const StringPiece& message);
		void Send(ChannelBuffer* message);  
		void ShutDownWriteForHalfClose();

	private:
		enum class ConnectionState { kDisconnected, kConnecting, kConnected, kDisconnecting };
		void HandleRead();
		void HandleWrite();
		void HandleClose();
		void HandleError();
		void SetState(ConnectionState s) { connectionState_ = s; }
		
		void SendStringPieceInLoop(const StringPiece& message) ;
		void SendDataInLoop(const void* message, size_t len);
		


	private:
		
		const InternetAddress& localAddress_;
		const InternetAddress& peerAddress_;
		Socket* socket_;
		EventLoop* loop_;
		ConnectionState connectionState_;
		const std::string connectionName_;
		Channel* connectionChannel_;
		TcpConnectionCallback connectionCallback_;
		TcpConnectionCallback connectionDestroyCallback_;
		TcpMessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;
		CloseCallback closeCallBack_;
		ChannelBuffer inputBuf_;
		ChannelBuffer outputBuf_;
	};
}


#endif