
#include "../CServerConfig.h"
#include "../Common/CommonFunc.h"

#include "CServiceBase.h"



CServiceBase::CServiceBase()
{
	m_pIPacketDispatcher = nullptr;
}


CServiceBase::~CServiceBase()
{

}

CServiceBase* CServiceBase::GetInstancePtr()
{
	static CServiceBase sObject;
	return &sObject;
}

bool CServiceBase::Start(std::string& strListenIp, UINT32 nPort, UINT32 nMaxConn, IPacketDispatcher* pIPacketDispatcher)
{
	if (nullptr == pIPacketDispatcher)
		return false;
	try
	{
		m_pIPacketDispatcher = (IPacketDispatcher*)pIPacketDispatcher;
	}
	catch (const std::exception&)
	{

	}

	CNetManager::GetInstancePtr()->Start(strListenIp, nPort, nMaxConn, this);

	return false;
}

bool CServiceBase::OnCloseConnect(UINT32 nConnID)
{
	return true;
}

bool CServiceBase::OnNewConnect(UINT32 nConnID)
{
	return true;
}
