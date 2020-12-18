#include "CNetManager.h"



CNetManager::CNetManager()
{
	m_pBufferHandler = nullptr;
}
CNetManager::~CNetManager()
{
	m_pBufferHandler = nullptr;
}

CNetManager* CNetManager::GetInstancePtr()
{
	static CNetManager sObject;
	return &sObject;
 }


bool CNetManager::Start(std::string& strListenIp, UINT32 nPort, UINT32 nMaxConn, IDataHandler* pBufferHandler)
{
	m_pBufferHandler = pBufferHandler;

	CNetSessionMrg::GetInstancePtr()->InitConnectionList(nMaxConn, m_IoService);

	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(strListenIp), nPort);
	m_pAcceptor = new boost::asio::ip::tcp::acceptor(m_IoService, ep, true);

	WaitForConnet();

	m_pWorkThread = new std::thread(boost::bind(&boost::asio::io_service::run, &m_IoService));
	return true;
}


void CNetManager::HandleAccept(CNetSession* NetSession_, const boost::system::error_code& e)
{
	if (e)
	{
		//这里是监听出错，需要处理．
		//NetSession_->Close();
		return;
	}
	m_pBufferHandler->OnNewConnect(NetSession_->GetSessionID());
	NetSession_->DoReceive();

	WaitForConnet();
}

bool CNetManager::WaitForConnet()
{
	CNetSession* CNetSession_ = CNetSessionMrg::GetInstancePtr()->CreateNetSession();
	if (!CNetSession_)return false;
	CNetSession_->SetDataHandler(m_pBufferHandler);
	m_pAcceptor->async_accept(CNetSession_->GetSocket(), boost::bind(&CNetManager::HandleAccept, this, CNetSession_, boost::asio::placeholders::error));
	return true;
}


//

void CNetManager::HandleConnect(CNetSession* pNetSession, const boost::system::error_code& e)
{
	if (e)
	{
		//pNetSession->Close();
		return;
	}
	m_pBufferHandler->OnNewConnect(pNetSession->GetSessionID());
	pNetSession->DoReceive();
}
CNetSession* CNetManager::ConnectTo_Async(std::string strIpAddr, UINT16 sPort)
{
	CNetSession* pNetSession = CNetSessionMrg::GetInstancePtr()->CreateNetSession();
	boost::asio::ip::tcp::resolver resolver(m_IoService);
	boost::asio::ip::tcp::resolver::query query(strIpAddr, CommonConvert::IntToString(sPort));
	boost::asio::ip::tcp::resolver::iterator enditor = resolver.resolve(query);
	pNetSession->SetDataHandler(m_pBufferHandler);
	boost::asio::async_connect(pNetSession->GetSocket(), enditor, boost::bind(&CNetManager::HandleConnect, this, pNetSession, boost::asio::placeholders::error));

	return pNetSession;
}


bool CNetManager::SendMessageData(UINT32 dwConnID, UINT32 dwMsgID, UINT64 u64TargetID, UINT32 dwUserData, const char* pData, UINT32 dwLen)
{
	CNetSession* pConn = CNetSessionMrg::GetInstancePtr()->GetNetSessionByID(dwConnID);
	if (nullptr == pConn)
	{
		return false;
	}
	//拼接数据包
	IDataBuffer* pDataBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(dwLen + sizeof(PacketHeader));
	PacketHeader* pHeader = (PacketHeader*)pDataBuffer->GetBuffer();
	pHeader->CheckCode = 0x99;//检查代码
	pHeader->dwUserData = dwUserData;
	pHeader->u64TargetID = u64TargetID;
	pHeader->dwSize = dwLen + sizeof(PacketHeader);
	pHeader->dwMsgID = dwMsgID;
	pHeader->dwPacketNo = 1;
	memcpy(pDataBuffer->GetBuffer() + sizeof(PacketHeader), pData, dwLen);
	pDataBuffer->SetTotalLenth(pHeader->dwSize);
	if (pConn->SendBuffer(pDataBuffer))
	{
		m_IoService.post(boost::bind(&CNetSession::DoSend, pConn));
	//	PostSendOperation(pConn);
	//	return TRUE;
	}
	return true;
}