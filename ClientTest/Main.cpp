#include "../ServerEngine/baseinet/CServiceBase.h"
#include "../ServerEngine/CServerConfig.h"
#include "../Common/CommonFunc.h"

class Client : public IPacketDispatcher
{
public:
	Client(){}
	virtual ~Client(){}

	bool OnCloseConnect(UINT32 nConnID)
	{
		return true;
	}
	bool OnNewConnect(UINT32 nConnID)
	{
		return true;
	}
	bool OnSecondTimer()
	{
		return true;
	}

	void Init()
	{
		ServerConfigInfo sSvr = CServerConfig::GetInstancePtr()->GetServerInfoByServerName("ServerTest");
		CServiceBase::GetInstancePtr()->Start(sSvr.Ip, sSvr.Prot, sSvr.MaxConct, this);

		ServerConfigInfo sCli = CServerConfig::GetInstancePtr()->GetServerInfoByServerName("ClientTest");
		CServiceBase::GetInstancePtr()->ConnectTo(sCli.Ip, sCli.Prot);
	}
};

int main()
{
	CommonFunc::SetWorkDir();

	Client* pClient = new Client;
	pClient->Init();

	std::cin.get();
	return 0;
}
