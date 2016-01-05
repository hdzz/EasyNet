#include "net/ProtobufFrameCodec.h"
#include "net/TcpConnection.h"
#include "net/Endian.h"


namespace Net
{
	const int32_t kHeaderLength = 4;
	ProtobufFrameCodec::ProtobufFrameCodec(const ::google::protobuf::Message* prototype,
		const ProtobufMessageCallback& messageCb,
		const ProtobufErrorCallback& errorCb)
		: protobufMessageCb_(messageCb)
		, protobufErrorCb_(errorCb)
	{

	}

	ProtobufFrameCodec::~ProtobufFrameCodec()
	{

	}

	void ProtobufFrameCodec::Encode(const TcpConnectionPtr& conn, const google::protobuf::Message& message)
	{
		std::string encodeResult;
		encodeResult.resize(kHeaderLen);
		const std::string& typeName = message.GetTypeName();
		//type name end with '\0'
		int32_t typeNameLength = typeName.size() + 1;
		int32_t newWorkTypeNameLen = Endian::HostToNetwork32(typeNameLength);
		encodeResult.append(reinterpret_cast<char*>(&newWorkTypeNameLen), typeNameLength);
		encodeResult.append(typeName.c_str(), typeNameLength);
		bool bRet = message.AppendToString(&encodeResult);
		if (bRet)
		{
			//TODO , add crc or checksum
			int32_t messageTypeAndContentLen = encodeResult.size() - kHeaderLen;
			std::copy(reinterpret_cast<char*>(&messageTypeAndContentLen),
				reinterpret_cast<char*>(&messageTypeAndContentLen) + sizeof messageTypeAndContentLen,
				encodeResult.begin());
		}
		else
		{
			encodeResult.clear();
		}
		ChannelBuffer buffer;
		buffer.Append(encodeResult);
		conn->Send(&buffer);
	}

	void ProtobufFrameCodec::Decode(const TcpConnectionPtr& conn, ChannelBuffer* buffer)
	{
		//wait util buffer contains valid protobuf message content
		while (buffer->ReadableBytes() < kMaxMessageLen && buffer->ReadableBytes() > (kHeaderLen * 2))
		{
			const int32_t messageTypeAndContentLen = buffer->ReadAsInt32(buffer->GetReadPointer());
			const int32_t messageTypeLen = buffer->ReadAsInt32(buffer->GetReadPointer() + 4);
			assert(messageTypeLen > 2 && messageTypeLen < messageTypeAndContentLen - 4);
			if (buffer->ReadableBytes() < (messageTypeAndContentLen + kHeaderLen))
			{
				//wait util receive the whole protobuf message, not pending here, will pending IO thread
				continue;
			}
			else
			{
				std::string messageTypeName(buffer->GetReadPointer() + kHeaderLen * 2,
					buffer->GetReadPointer() + kHeaderLen * 2 + messageTypeLen);

				MessagePtr msg(CreateProtobufMessage(messageTypeName));
				assert(msg.get());
				msg->ParseFromArray(buffer->GetReadPointer() + kHeaderLen * 2 + messageTypeLen,
					messageTypeAndContentLen - messageTypeLen);
				protobufMessageCb_(conn, msg);
				buffer->Pop(kHeaderLen + messageTypeAndContentLen);
			}
			break;
		}
	}

	google::protobuf::Message* ProtobufFrameCodec::CreateProtobufMessage(const std::string& messageTypeName)
	{
		::google::protobuf::Message* message = nullptr;
		const google::protobuf::Descriptor* descriptor = 
			google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(messageTypeName);
		if (descriptor)
		{
			auto protoType = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
			if (protoType)
			{
				message = protoType->New();
			}
		}
		return message;
	}



}