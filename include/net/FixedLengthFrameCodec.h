#ifndef FIXED_LENGTH_FRAME_CODEC_H__
#define FIXED_LENGTH_FRAME_CODEC_H__
#include "net/Codec.h"


namespace Net
{
	class FixedLengthFrameCodec final : public Codec
	{
	public:
		FixedLengthFrameCodec(const CodecMessageCallback& cb, int32_t iFrameDecodeLength);
		~FixedLengthFrameCodec();

		virtual void Encode(const TcpConnectionPtr& conn, const StringPiece& msg) override;
		void Decode(const TcpConnectionPtr& conn, ChannelBuffer* buffer) override;


	private:
		int32_t frameLength_;

	};
}





#endif