#ifndef CHANNEL_BUFFER_H__
#define CHANNEL_BUFFER_H__

#include <stdint.h>
#include <vector>
#include <assert.h>
#include "net/Endian.h"

namespace Net
{
	extern const char kEOL;
	const uint8_t kBufferCheapPrepend = 8;
	const uint64_t kBufferInitialSize = 1024;
	class ChannelBuffer
	{
	public:
		ChannelBuffer()
			: buffer_(kBufferCheapPrepend + kBufferInitialSize)
			, writeIndex_(kBufferCheapPrepend)
			, readIndex_(kBufferCheapPrepend)
		{

		}
		const char* GetReadPointer() { return Begin() + readIndex_; }

		char* GetWriteablePointer() { return Begin() + writeIndex_; }

		uint64_t ReadableBytes(){ return writeIndex_ - readIndex_; }

		uint64_t WriteableBytes() { return Size() - writeIndex_; }

		uint64_t PrependbaleBytes(){ return readIndex_; }

		void PrependHeader(const void* data, int dataLen);

		void Append(const std::string& buf){ Append(buf.c_str(), strlen(buf.c_str())); }

		std::string ReadAllAsString() { return ReadBytesAsString(ReadableBytes()); }

		std::string ReadBytesAsString(uint64_t len);

		std::string ReadContentsUntilLastEOF()
		{
			auto lastEofPositon = FindLastEndOfLine();
			if (lastEofPositon)
			{
				return ReadBytesAsString(FindLastEndOfLine() - GetReadPointer());
			}
			return nullptr;
		}

		std::string ReadLine()
		{
			auto firstEofPositon = FindFirstEndOfLine();
			if (firstEofPositon)
			{
				int32_t singleLineLen = FindFirstEndOfLine() - GetReadPointer() + 1;
				return ReadBytesAsString(singleLineLen);
			}
			return nullptr;
		}



		int64_t ReadFromSocketFD(uint32_t sockFd, int& error);


		template <class T>
		void Append(T* dataPtr, uint64_t len)
		{
			if (len > WriteableBytes())
			{
				AssureEnoughSpace(len);
			}
			assert(WriteableBytes() >= len);
			::memcpy(GetWriteablePointer(), dataPtr, len);
			writeIndex_ += len;
		}

		void Pop(uint64_t len);

		void PopInt32() { Pop(sizeof(int32_t)); };
		void PopInt64() { Pop(sizeof(int64_t)); };

		int32_t ReadAsInt32(const char* buf)
		{
			int32_t be32 = 0;
			::memcpy(&be32, buf, sizeof(be32));
			return Endian::NetworkToHost32(be32);
		}

		char* Begin() { return &*buffer_.begin(); }
		char* End() { return &*buffer_.end(); }
		uint64_t Size() { return buffer_.size(); }

	private:
		void AssureEnoughSpace(uint64_t len);

		const char* FindLastEndOfLine();
		const char* FindFirstEndOfLine();



	private:
		std::vector<char> buffer_;
		uint64_t writeIndex_;
		uint64_t readIndex_;
	};
}

#endif