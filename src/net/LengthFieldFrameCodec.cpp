#include "net/LengthFieldFrameCodec.h"
#include "net/TcpConnection.h"
#include "net/ChannelBuffer.h"
#include "net/Endian.h"
#include "base/StringPiece.h"

#include <iostream>




namespace Net
{
	LengthFieldBasedFrameDecoder::LengthFieldBasedFrameDecoder(const CodecMessageCallback& cb)
		: Codec(cb)
	{

	}

	LengthFieldBasedFrameDecoder::~LengthFieldBasedFrameDecoder()
	{

	}

	void LengthFieldBasedFrameDecoder::Encode(const TcpConnectionPtr& conn, const StringPiece& msg)
	{
		ChannelBuffer buffer;
		buffer.Append(msg.as_string());
		int msgLen = msg.size();
		int32_t be32 = Endian::HostToNetwork32(msgLen);
		buffer.PrependHeader(&be32, 4);
		conn->Send(&buffer);
	}

	void LengthFieldBasedFrameDecoder::Decode(const TcpConnectionPtr& conn, ChannelBuffer* buffer)
	{
		const void* headerAddress = buffer->GetReadPointer();
		int32_t networkEndian = *static_cast<const int32_t*>(headerAddress);
		const int32_t message_len = Endian::NetworkToHost32(networkEndian);
		if (buffer->ReadableBytes() >= message_len + 4)
		{
			buffer->Pop(4); //remove header from buffer
			std::string message = buffer->ReadBytesAsString(message_len);
			codecMsgCb_(conn, std::move(message));
		}

	}
}