#include "net/TcpConnection.h"
#include "net/EventLoop.h"
#include <functional>
#include <iostream>


namespace Net
{

	void DefaultConnectionCallback(const TcpConnectionPtr& conn)
	{
		std::cout << "Connection: " << conn->GetPeerAddress().GetIpAndPort() << " -> "
			<< conn->GetLocalAddress().GetIpAndPort()
			<< (conn->IsConnected() ? " established..." : " destroyed...")
			<<  std::endl;
	}

	void DefaultMessageCallback(const TcpConnectionPtr&, ChannelBuffer* buf)
	{
		buf->ReadAllAsString();
	}

	TcpConnection::TcpConnection(EventLoop* loop,
		const std::string& name,
		int sockfd,
		const InternetAddress& localAddr,
		const InternetAddress& peerAddr)
		: loop_(loop)
		, connectionState_(ConnectionState::kConnecting)
		, connectionName_(name)
		, socket_(new Socket(sockfd))
		, localAddress_(localAddr)
		, peerAddress_(peerAddr)
		, connectionChannel_ (new Channel(loop, sockfd))
	{
		connectionChannel_->SetReadCallBack(std::bind(&TcpConnection::HandleRead, this));
		connectionChannel_->SetWriteCallBack(std::bind(&TcpConnection::HandleWrite, this));
	}

	TcpConnection::~TcpConnection()
	{
		std::cout << "TcpConnection dtor..." << std::endl;
	}


	void TcpConnection::Send(const StringPiece& message)
	{
		if (connectionState_ == ConnectionState::kConnected)
		{
			if (loop_->IsInEventLoopOwnThread())
			{
				SendStringPieceInLoop(message);
			}
			else
			{
				loop_->PushAsyncTaskInLoop(nullptr);
			}
		}
	}

	void TcpConnection::Send(ChannelBuffer* message)
	{
		if (connectionState_ == ConnectionState::kConnected)
		{
			if (loop_->IsInEventLoopOwnThread())
			{
				SendStringPieceInLoop(message->ReadAllAsString());
			}
			else
			{
				loop_->PushAsyncTaskInLoop(nullptr);

				loop_->PushAsyncTaskInLoop(std::bind(&TcpConnection::SendStringPieceInLoop,
					this, message->ReadAllAsString()));
			}
		}
	}


	void TcpConnection::SendStringPieceInLoop(const StringPiece& message)
	{
		SendDataInLoop(message.data(), message.size());
	}

	void TcpConnection::SendDataInLoop(const void* data, size_t len)
	{
		loop_->AssertInLoopThreadOrDie();
		size_t nwrote = 0;
		size_t remaining = len;
		bool faultError = false;
		if (connectionState_ == ConnectionState::kDisconnected)
		{
			//LOG_WARN << "disconnected, give up writing";
			return;
		}
		// if nothing in output queue, try writing directly
		if (!connectionChannel_->IsWriting() && outputBuf_.ReadableBytes() == 0)
		{
			nwrote = SocketsApi::Send(connectionChannel_->GetSocketFd(), data, len);
			if (nwrote >= 0)
			{
				remaining = len - nwrote;
				if (remaining == 0 && writeCompleteCallback_)
				{
					loop_->PushAsyncTaskInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
				}
			}
			else // nwrote < 0
			{
				nwrote = 0;
				if (errno != EWOULDBLOCK)
				{
					//LOG_SYSERR << "TcpConnection::sendInLoop";
					if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
					{
						faultError = true;
					}
				}
			}
		}

		assert(remaining <= len);
		if (!faultError && remaining > 0)
		{
			outputBuf_.Append(static_cast<const char*>(data)+nwrote, remaining);
			if (!connectionChannel_->IsWriting())
			{
				connectionChannel_->SetInterestEvent(EventType::kWriteEvent);
			}
		}
	}


	void TcpConnection::ShutDownWriteForHalfClose()
	{
		loop_->AssertInLoopThreadOrDie();
		// shutDown write for half close, only can read at this time.
		if (!connectionChannel_->IsWriting())
		{
			socket_->ShutdownWrite();
		}
	}


	void TcpConnection::HandleRead()
	{
		int tmpError = 0;
		int n = inputBuf_.ReadFromSocketFD(connectionChannel_->GetSocketFd(), tmpError);
		if (n > 0)
		{
			messageCallback_(shared_from_this(), &inputBuf_);
		}
		else if (n == 0)
		{
			HandleClose();
		}
		else
		{
			std::cout << "TcpConnection::handleRead, Error..." << std::endl;
			HandleClose();
		}
	}


	void TcpConnection::HandleWrite()
	{
		if (connectionChannel_->IsWriting())
		{
			int n = SocketsApi::Send(connectionChannel_->GetSocketFd(), 
									outputBuf_.GetReadPointer(), 
									outputBuf_.ReadableBytes());
			if (n > 0)
			{
				outputBuf_.Pop(n);
				if (outputBuf_.ReadableBytes() == 0)
				{
					connectionChannel_->DisableWriting();
					if (writeCompleteCallback_)
					{
						loop_->PushAsyncTaskInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
					}
					if (connectionState_ == ConnectionState::kDisconnecting)
					{
						ShutDownWriteForHalfClose();
					}
				}
			}
			else if (n == 0)
			{
				//write nothing
			}
			else
			{
				//TODO, error handler
			}
		}
		else
		{
			std::cout << "Connection fd = " << connectionChannel_->GetSocketFd()
				<< " is down, can not write any more...";
		}
		
	}

	void TcpConnection::HandleClose()
	{
		loop_->AssertInLoopThreadOrDie();
		assert(connectionState_ == ConnectionState::kConnected 
			|| connectionState_ == ConnectionState::kDisconnecting);
		SetState(ConnectionState::kDisconnected);
		connectionChannel_->DisableAll();
		TcpConnectionPtr thisConn(shared_from_this());
		closeCallBack_(thisConn);
	}


	void TcpConnection::HandleError()
	{
		int err = SocketsApi::GetSocketError(connectionChannel_->GetSocketFd());

	}

	void TcpConnection::ConnectionEstablished()
	{
		loop_->AssertInLoopThreadOrDie();
		assert(connectionState_ == ConnectionState::kConnecting);
		SetState(ConnectionState::kConnected);
		connectionChannel_->SetInterestEvent(EventType::kReadEvent);
		connectionCallback_(shared_from_this());
	}

	void TcpConnection::ConnectionDestroyed()
	{
		loop_->AssertInLoopThreadOrDie();
		if (connectionState_ == ConnectionState::kConnected)
		{
			SetState(ConnectionState::kDisconnected);
			connectionChannel_->SetInterestEvent(EventType::kInvalidEvent);
			connectionDestroyCallback_(shared_from_this());
		}
		connectionChannel_->Remove();
	}
}