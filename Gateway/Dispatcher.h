#pragma once


#include "../ProtobufMsg/Gateway/Login.pb.h"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

class delegate
{
public:
	delegate(): object_ptr(0), stub_ptr(0)
	{}

	template <class T, void (T::* TMethod)(unsigned long, google::protobuf::Message*)>
	static delegate from_method(T* object_ptr)
	{
		delegate d;
		d.object_ptr = object_ptr;
		d.stub_ptr = &method_stub<T, TMethod>; // #1
		return d;
	}

	void operator()(unsigned long lClientId, google::protobuf::Message* a1) const
	{
		return (*stub_ptr)(object_ptr, lClientId, a1);
	}

private:
	typedef void (*stub_type)(void* object_ptr, unsigned long, google::protobuf::Message*);

	void* object_ptr;
	stub_type stub_ptr;

	template <class T, void (T::* TMethod)(unsigned long, google::protobuf::Message*)>
	static void method_stub(void* object_ptr, unsigned long lClientId, google::protobuf::Message* a1)
	{
		T* p = static_cast<T*>(object_ptr);
		return (p->*TMethod)(lClientId, a1); // #2
	}
};

class CMsgDispatcher
{
public:
	CMsgDispatcher():m_bCanceled(false)
	{
		m_defaultCallback = delegate::from_method<CMsgDispatcher, &CMsgDispatcher::discardProtobufMessage>(this);
	}
	virtual ~CMsgDispatcher()
	{

	}
	void discardProtobufMessage(unsigned long lClientId, google::protobuf::Message* message)
	{
		//syslog("Discarding client:%d, msg:%s", lClientId, message->GetTypeName().c_str());
		std::cout << "Discarding client:%d, msg:%s" << lClientId << message->GetTypeName().c_str() << std::endl;
	}
	void CancelDispatch()
	{
		m_bCanceled = true;
	}

	void ResumeDispatch()
	{
		m_bCanceled = false;
	}

	bool DispatchIsCanceled()const
	{
		return m_bCanceled;
	}      
	google::protobuf::Message* createMessage(const std::string& type_name);
	google::protobuf::Message* Parse(const void* pData, unsigned int uSize);
	bool OnceDispatch(unsigned long lClientId, const void* pData, unsigned int uSize);

	template<typename T>
	void registerMessageCallback(delegate d)
	{
		m_callbacks[T::descriptor()] = d;
	}
private:
	bool m_bCanceled;
	typedef std::map<const google::protobuf::Descriptor*, delegate> CallbackMap;
	CallbackMap m_callbacks;
	delegate m_defaultCallback;
};

