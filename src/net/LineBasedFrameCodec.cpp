#include "net/LineBasedFrameCodec.h"
#include "net/TcpConnection.h"
#include "net/ChannelBuffer.h"
#include "base/StringPiece.h"
#include <iostream>

namespace Net
{
	
	LineBasedFrameCodec::LineBasedFrameCodec(const CodecMessageCallback& cb)
		: Codec(cb)
	{

	}


	LineBasedFrameCodec::~LineBasedFrameCodec()
	{

	}

	void LineBasedFrameCodec::Decode(const TcpConnectionPtr& conn, ChannelBuffer* buffer)
	{
		std:;string lineMsg = buffer->ReadLine();
		if (codecMsgCb_)
		{
			codecMsgCb_(conn, lineMsg);
		}
	}

	void LineBasedFrameCodec::Encode(const TcpConnectionPtr& conn, const StringPiece& msg)
	{
		const std::string tmpMsg = msg.as_string() + kEOL;
		ChannelBuffer buf;
		buf.Append(tmpMsg);
		conn->Send(&buf);
	}
}