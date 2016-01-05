#ifndef PROTOBUF_FRAME_CODEC_H__
#define	PROTOBUF_FRAME_CODEC_H__


#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include <string>
#include <functional>
#include "net/Codec.h"
#include "net/ICallBackDefine.h"
#include "net/TcpConnection.h"



namespace Net									  
{
	class ChannelBuffer;
	enum class ErrorCode
	{
		kNoError = 0,
		kInvalidLength,
		kCheckSumError,
		kInvalidNameLen,
		kUnknownMessageType,
		kParseError,
	};

	void DefaultErrorCallback(const TcpConnectionPtr& conn,
		ChannelBuffer* buf,
		const ErrorCode& errorCode)
	{
		if (conn && conn->IsConnected())
		{
			conn->ShutDownWriteForHalfClose();
		}
	}

	using MessagePtr = std::shared_ptr<google::protobuf::Message>;
	using ProtobufMessageCallback = std::function<void(const TcpConnectionPtr&, const MessagePtr&)> ;
	using ProtobufErrorCallback = std::function<void(const TcpConnectionPtr, ChannelBuffer*, const ErrorCode&)>;

	class ProtobufFrameCodec 
	{
	public:
		ProtobufFrameCodec(const google::protobuf::Message* prototype,
			const ProtobufMessageCallback& messageCb,
			const ProtobufErrorCallback& errorCb = DefaultErrorCallback);
		~ProtobufFrameCodec();

		

		void Encode(const TcpConnectionPtr& conn, const google::protobuf::Message& message);
		void Decode(const TcpConnectionPtr& conn, ChannelBuffer* buffer);

	private:
		google::protobuf::Message* CreateProtobufMessage(const std::string& messageTypeName);

	private:
		const static int kHeaderLen = sizeof(int32_t);
		const static int kChecksumLen = sizeof(int32_t);
		const static int kMaxMessageLen = 64 * 1024 * 1024;
		ProtobufMessageCallback protobufMessageCb_;
		ProtobufErrorCallback protobufErrorCb_;

	};
}

#endif


