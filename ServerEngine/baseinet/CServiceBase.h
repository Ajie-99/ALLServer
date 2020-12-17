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
public:
	virtual bool			OnCloseConnect(UINT32 nConnID);
	virtual bool			OnNewConnect(UINT32 nConnID);
	IPacketDispatcher* m_pIPacketDispatcher;
private:
};