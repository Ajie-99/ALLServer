#pragma once
#include "../PCH.h"
#include "../BaseDefine.h"

class CServiceBase : public IDataHandler
{
private:
	CServiceBase();
	~CServiceBase();
public:
	static	CServiceBase*	GetInstancePtr();
	bool					Start(std::string& strListenIp, UINT32 nPort, UINT32 nMaxConn, void *pIPacketDispatcher);
private:


	IPacketDispatcher*		m_pIPacketDispatcher;
	
};