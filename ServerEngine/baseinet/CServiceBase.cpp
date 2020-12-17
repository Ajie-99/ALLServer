
#include "../Platform.h"
#include "../CServerConfig.h"
#include "../Common/CommonFunc.h"
#include "CServiceBase.h"
#include "CNetManager.h"


CServiceBase::CServiceBase()
{
	m_pIPacketDispatcher = nullptr;
}


CServiceBase::~CServiceBase()
{

}

CServiceBase* CServiceBase::GetInstancePtr()
{
	static CServiceBase* sObject;
	return sObject;
}

bool CServiceBase::Start(std::string& strListenIp, UINT32 nPort, UINT32 nMaxConn, void* pIPacketDispatcher)
{
	m_pIPacketDispatcher = static_cast<IPacketDispatcher*> (pIPacketDispatcher);

	CNetManager::GetInstancePtr()->Start(strListenIp, nPort, nMaxConn, this);

	return false;
}