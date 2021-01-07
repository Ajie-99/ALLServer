#pragma once
#include "Dispatcher.h"

class CMsgDispacher : public CMsgDispatcher
{
public:
	CMsgDispacher();
	//virtual ~CMsgDispacher(){}
	#define DEFINE_MESSAGE(msg) /*virtual*/ void OnSync##msg(unsigned long lClientId, google::protobuf::Message* m)
	DEFINE_MESSAGE(LoginInfo);
	DEFINE_MESSAGE(LoginOut);
};

