#ifndef LENGTH_FIELD_FRAME_CODEC_H__
#define LENGTH_FIELD_FRAME_CODEC_H__
#include "net/Codec.h"

namespace Net
{
	//This class encapsute the codec which for the header-constrained package//
	class LengthFieldBasedFrameDecoder final: public Codec
	{
	public:
		LengthFieldBasedFrameDecoder(const CodecMessageCallback& cb);
		~LengthFieldBasedFrameDecoder();

		virtual void Encode(const TcpConnectionPtr& conn, const StringPiece& msg) override;
		void Decode(const TcpConnectionPtr& conn, ChannelBuffer* buffer) override;

	};
}


#endif