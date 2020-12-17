#include "../ServerEngine/CServerConfig.h"
#include "../Common/CommonFunc.h"

#include "CGameService.h"


CGameService::CGameService()
{
	CommonFunc::SetWorkDir();
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

	ServerConfigInfo sClf = CServerConfig::GetInstancePtr()->GetServerInfoByServerName("GatWay");
	CServiceBase::GetInstancePtr()->Start(sClf.Ip, sClf.Prot, sClf.MaxConct, this);
	return bRet;
}


bool CGameService::OnCloseConnect(UINT32 nConnID)
{
	return true;
}

bool CGameService::OnNewConnect(UINT32 nConnID)
{

	return true;
}
bool CGameService::OnSecondTimer()
{
	return true;
}