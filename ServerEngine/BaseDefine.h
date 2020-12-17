#pragma once
#include "PCH.h"


class IDataHandler
{
public:
	//virtual bool OnDataHandle(IDataBuffer* pDataBuffer, UINT32 nConnID) = 0;
	virtual bool OnCloseConnect(UINT32 nConnID) = 0;
	virtual bool OnNewConnect(UINT32 nConnID) = 0;
};

class IPacketDispatcher
{
public:
	//virtual BOOL DispatchPacket(NetPacket* pNetPacket) = 0;
	virtual bool OnCloseConnect(UINT32 nConnID) = 0;
	virtual bool OnNewConnect(UINT32 nConnID) = 0;
	virtual bool OnSecondTimer() = 0;
};

#pragma pack(1)

struct ServerConfigInfo
{
	UINT32	ServerId;
	UINT16	Prot;
	UINT32	MaxConct;
	std::string	ServerName;
	std::string	Ip;
};



#pragma pack()