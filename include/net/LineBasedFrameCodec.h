#ifndef LINE_BASED_FRAME_CODEC
#define LINE_BASED_FRAME_CODEC


#include "Codec.h"
namespace Net
{
	//This class encapsute the codec which for the line-based package//
	class LineBasedFrameCodec final : public Codec
	{
	public:
		LineBasedFrameCodec(const CodecMessageCallback& cb);
		~LineBasedFrameCodec();

		virtual void Decode(const TcpConnectionPtr& conn, ChannelBuffer* buffer) override;
		virtual void Encode(const TcpConnectionPtr& conn, const StringPiece& msg) override;

	};
}


#endif