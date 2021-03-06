#include "../ServerEngine/CServerConfig.h"
#include "../Common/CommonFunc.h"

#include "CGameService.h"

#include "../ProtobufMsg/Gateway/Login.pb.h"

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
	bRet = CServiceBase::GetInstancePtr()->Start(sClf.Ip, sClf.Prot, sClf.MaxConct, this);
	return bRet;
}

bool CGameService::Run()
{
	for (;;)
	{
		CServiceBase::GetInstancePtr()->Update();
	}
}
//////////////////////////////////////////////////////////////////////////
bool CGameService::OnCloseConnect(UINT32 nConnID)
{
	return true;
}

bool CGameService::OnNewConnect(UINT32 nConnID)
{
	std::cout << "�����ӽ���  :" << nConnID << "   " << std::endl;

	return true;
}
bool CGameService::OnSecondTimer()
{
	return true;
}

bool CGameService::DispatchPacket(NetPacket* pNetPacket)
{
	rpc::LoginInfo Login{};
	Login.ParsePartialFromArray(pNetPacket->m_pDataBuffer->GetData(), pNetPacket->m_pDataBuffer->GetBodyLenth());
	return true;
}