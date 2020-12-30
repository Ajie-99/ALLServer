#pragma once
#include "../ServerEngine/baseinet/CServiceBase.h"

class CGameService : public IPacketDispatcher
{
private:
	CGameService();
	virtual ~CGameService();
public:
	static CGameService* GetInstancePtr();
	bool Init();
	bool Run();
public:
	virtual bool OnCloseConnect(UINT32 nConnID);
	virtual bool OnNewConnect(UINT32 nConnID);
	virtual bool OnSecondTimer();
	virtual bool DispatchPacket(NetPacket* pNetPacket);
};

