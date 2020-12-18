#pragma once
#include "CNetManager.h"
class CServiceBase : public IDataHandler
{
public:
	CServiceBase();
	virtual ~CServiceBase();
public:
	static	CServiceBase*	GetInstancePtr();
	bool					Start(std::string& strListenIp, UINT32 nPort, UINT32 nMaxConn, IPacketDispatcher*pIPacketDispatcher);
	bool					Update();
public:
	virtual bool			OnCloseConnect(UINT32 nConnID);
	virtual bool			OnNewConnect(UINT32 nConnID);
public:
	CNetSession*			ConnectTo(std::string strIpAddr, UINT16 sPort);


	bool SendMsgProtoBuf(UINT32 dwConnID, UINT32 dwMsgID, UINT64 u64TargetID, UINT32 dwUserData, std::string& pdata/*const google::protobuf::Message& pdata*/);
private:
	std::deque<NetPacket>*		m_pRecvDataQueue;
	std::deque<NetPacket>*		m_pDispathQueue;
	IPacketDispatcher*			m_pIPacketDispatcher;
	std::mutex					m_QueueLock;
};