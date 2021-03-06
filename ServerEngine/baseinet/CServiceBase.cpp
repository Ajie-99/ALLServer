
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
				std::cout << "---派发消息---" << Item .m_pDataBuffer<< std::endl;
				m_pIPacketDispatcher->DispatchPacket(&Item);
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


bool CServiceBase::SendMsgProtoBuf(UINT32 dwConnID, UINT32 dwMsgID, UINT64 u64TargetID, UINT32 dwUserData, std::string& pdata/*const google::protobuf::Message& pdata*/)
{
	//暂时这样写
	CNetManager::GetInstancePtr()->SendMessageData(dwConnID, dwMsgID, u64TargetID, dwUserData, pdata.c_str(), pdata.length());

	return true;
}
//////////////////////////////////////////////////////////////////////////

bool CServiceBase::OnCloseConnect(UINT32 nConnID)
{
	m_QueueLock.lock();
	m_pRecvDataQueue->push_back(NetPacket(nConnID, CLOSE_CONNECTION, nullptr));
	m_QueueLock.unlock();
	return true;
}

bool CServiceBase::OnNewConnect(UINT32 nConnID)
{
	m_QueueLock.lock();
	m_pRecvDataQueue->push_back(NetPacket(nConnID, NEW_CONNECTION, nullptr));
	m_QueueLock.unlock();
	return true;
}
bool	CServiceBase::OnDataHandle(IDataBuffer* pDataBuffer, UINT32 header_size, UINT32 nConnID)
{
	//std::cout << "---------------------------------------------" << std::endl;
	//char* bbuf = pDataBuffer->GetBuffer() + header_size;
	//int nLen = pDataBuffer->GetTotalLenth() - header_size;
	//char* length = pDataBuffer->GetBuffer() + header_size - 4;
	//CHttpParser::parser_ws_pack((unsigned char *)bbuf, nLen, (unsigned char*)length);

	m_QueueLock.lock();
	m_pRecvDataQueue->emplace_back(NetPacket(nConnID,0, pDataBuffer));
	m_QueueLock.unlock();

	return true;
}