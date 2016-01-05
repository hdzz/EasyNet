#ifndef CODEC_H__
#define CODEC_H__

#include <functional>
#include "ICallBackDefine.h"
#include "base/StringPiece.h"

namespace Net
{
	class Codec
	{
	public:
		Codec(const CodecMessageCallback& cb) :codecMsgCb_(cb){}
		virtual ~Codec() {};

	private:
		virtual void Decode(const TcpConnectionPtr& conn, ChannelBuffer* buffer) = 0;
		virtual void Encode(const TcpConnectionPtr& conn, const StringPiece& msg) = 0;

	protected: 
		CodecMessageCallback codecMsgCb_;

	};
}



#endif