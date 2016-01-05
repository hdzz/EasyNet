#include "net/ChannelBuffer.h"
#include "net/SocketApiWrapper.h"
#include <iostream>
#include <string.h>
#include <algorithm>


namespace Net
{
	const char kEOL = '\n';
	//const char kEOL = '\r\n';
	const int kreadBytesEveryTime = 1024;
	namespace details
	{
		const void* memrchar(const char* sourceBuf, int findChar, uint64_t iFindNum)
		{
#ifdef ON_LINUX
			return ::memrchr(sourceBuf, findChar, iFindNum);
#else 
			while (iFindNum >= 0 && *(sourceBuf + iFindNum - 1) != findChar)
			{
				if (iFindNum-- == 0)
				{
					return nullptr;
				}
			}
			return sourceBuf + iFindNum;
#endif
		}
	}

	void ChannelBuffer::AssureEnoughSpace(uint64_t len)
	{
		if (len > (WriteableBytes() + PrependbaleBytes() - kBufferCheapPrepend))
		{
			buffer_.resize(writeIndex_ + len);
		}
		else
		{
			//do not need to allocate new memory spaces, just move the data forward
			uint64_t readableBytes = ReadableBytes();
			::memcpy(Begin() + kBufferCheapPrepend, GetReadPointer(), readableBytes);
			readIndex_ = kBufferCheapPrepend;
			writeIndex_ = readIndex_ + readableBytes;
		}
	}

	void ChannelBuffer::Pop(uint64_t len)
	{
		assert(len <= ReadableBytes());
		if (len < ReadableBytes())
		{
			readIndex_ += len;
		}
		else
		{
			writeIndex_ = readIndex_ = kBufferCheapPrepend;
		}
	}

	std::string ChannelBuffer::ReadBytesAsString(uint64_t len)
	{
		assert(len <= ReadableBytes());
		std::string result(GetReadPointer(), len);
		Pop(len);
		return result;
	}

	const char* ChannelBuffer::FindLastEndOfLine()
	{
		return static_cast<const char*>
			(details::memrchar(GetReadPointer(), kEOL, ReadableBytes()));
	}


	const char* ChannelBuffer::FindFirstEndOfLine()
	{
		return static_cast<const char*>(memchr(GetReadPointer(), kEOL, ReadableBytes()));
	}

	void ChannelBuffer::PrependHeader(const void* data, int dataLen)
	{
		assert(readIndex_ >= dataLen);
		const char* d = static_cast<const char*>(data);
		readIndex_ -= dataLen;
		std::copy(d, d + dataLen, Begin() + readIndex_);
	}

	int64_t ChannelBuffer::ReadFromSocketFD(uint32_t sockFd, int& error)
	{
		//TODO, read data to the ChannelBuffer directly, no need to read to the temp buffer//
		char buffer[kreadBytesEveryTime] = { '\0' };
		int64_t readBytes = SocketsApi::Receive(sockFd, buffer, kreadBytesEveryTime);
		if (readBytes > 0)
		{
			Append(buffer, readBytes);
		}
		else if (readBytes == 0)
		{
			std::cout << "readBytes is 0...." << std::endl;
		}
		else
		{
			error = errno;
			std::cout << "readBytes is less than 0...." << std::endl;
		}
		return readBytes;
	}
	
}