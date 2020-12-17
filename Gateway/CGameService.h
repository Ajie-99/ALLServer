#pragma once
#include "../ServerEngine/PCH.h"
#include "../ServerEngine/BaseDefine.h"

class CGameService : public IPacketDispatcher
{
private:
	CGameService();
	~CGameService();
public:
	static CGameService* GetInstancePtr();
	bool Init();
public:
	virtual bool OnCloseConnect(UINT32 nConnID)override;
	virtual bool OnNewConnect(UINT32 nConnID)override;
	virtual bool OnSecondTimer()override;

};

