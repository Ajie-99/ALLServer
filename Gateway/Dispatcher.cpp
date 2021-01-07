#include "Dispatcher.h"



google::protobuf::Message* CMsgDispatcher::createMessage(const std::string& type_name)
{
	google::protobuf::Message* message = NULL;
	const google::protobuf::Descriptor* descriptor =
		google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
	if (descriptor)
	{
		const google::protobuf::Message* prototype =
			google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype)
		{
			message = prototype->New();
		}
	}
	return message;
}


google::protobuf::Message* CMsgDispatcher::Parse(const void* pData, unsigned int uSize)
{
	rpc::Request req;
	if (uSize < sizeof(unsigned int))
		return NULL;

	if (!req.ParseFromArray(pData, uSize))
		return NULL;

	google::protobuf::Message* message = createMessage(req.method());
	if (message)
		message->ParseFromArray(req.serialized_request().c_str(), req.serialized_request().length());

	return message;
}


bool CMsgDispatcher::OnceDispatch(unsigned long lClientId, const void* pData, unsigned int uSize)
{
	google::protobuf::Message* message = Parse(pData, uSize);
	if (message)
	{
		CallbackMap::const_iterator it = m_callbacks.find(message->GetDescriptor());
		if (it != m_callbacks.end())
			it->second(lClientId, message);
		else
			m_defaultCallback(lClientId, message);

		delete message;

		return true;
	}

	return false;
}