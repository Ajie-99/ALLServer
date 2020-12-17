#include "CGameService.h"
#include "../ServerEngine/baseinet/CServiceBase.h"
#include "../ServerEngine/CServerConfig.h"

CGameService::CGameService()
{

}

CGameService::~CGameService()
{

}

CGameService* CGameService::GetInstancePtr()
{
	static CGameService sObject;
	return &sObject;
}


bool CGameService::Init()
{
	bool bRet = false;

	ServerConfigInfo Clf = CServerConfig::GetInstancePtr()->GetServerInfoByServerName("GatWay");
	bRet = CServiceBase::GetInstancePtr()->Start(Clf.Ip, Clf.Prot, Clf.MaxConct, this);
	return bRet;
}


bool CGameService::OnCloseConnect(UINT32 nConnID)
{
	return false;
}

bool CGameService::OnNewConnect(UINT32 nConnID)
{

	return false;
}
bool CGameService::OnSecondTimer()
{
	return false;
}