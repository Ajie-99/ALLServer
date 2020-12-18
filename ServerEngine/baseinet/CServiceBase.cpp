
#include "../CServerConfig.h"
#include "../Common/CommonFunc.h"

#include "CServiceBase.h"



CServiceBase::CServiceBase()
{
	m_pIPacketDispatcher = nullptr;
	m_pRecvDataQueue = new std::deque<NetPacket>();
	m_pDispathQueue = new std::deque<NetPacket>();
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

	m_pIPacketDispatcher = (IPacketDispatcher*)pIPacketDispatcher;


	CNetManager::GetInstancePtr()->Start(strListenIp, nPort, nMaxConn, this);

	return true;
}

bool		CServiceBase::Update()
{
	m_QueueLock.lock();
	std::swap(m_pRecvDataQueue, m_pDispathQueue);
	m_QueueLock.unlock();
	//m_pDispathQueue = std::move(m_pRecvDataQueue);
	if (m_pDispathQueue->size() > 0)
	{
		auto Iter = m_pDispathQueue->begin();
		for (;Iter != m_pDispathQueue->end(); Iter++)
		{
			NetPacket& Item = *Iter;
			switch (Item.m_dwMsgID)
			{
			case NEW_CONNECTION:
				std::cout << "---新消息来了---" << Item.m_dwConnID<< std::endl;
				m_pIPacketDispatcher->OnNewConnect(Item.m_dwConnID);
				break;
			case CLOSE_CONNECTION:
				std::cout << "---新消息关闭---" << std::endl;
				break;
			default:
				break;
			}
		}
		m_pDispathQueue->clear();
	}


	return true;
}

CNetSession* CServiceBase::ConnectTo(std::string strIpAddr, UINT16 sPort)
{
	return CNetManager::GetInstancePtr()->ConnectTo_Async(strIpAddr, sPort);
}

//////////////////////////////////////////////////////////////////////////

bool CServiceBase::OnCloseConnect(UINT32 nConnID)
{
	return true;
}

bool CServiceBase::OnNewConnect(UINT32 nConnID)
{
	std::cout << "有链接进来  :" << nConnID << "   " << std::endl;

	m_pRecvDataQueue->push_back(NetPacket(nConnID, NEW_CONNECTION, nullptr));

	return true;
}
