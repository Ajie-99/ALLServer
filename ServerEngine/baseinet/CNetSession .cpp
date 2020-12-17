#include "../PCH.h"
#include "../Platform.h"
#include "CNetSession .h"
#include "../BaseDefine.h"


CNetSession::CNetSession(boost::asio::io_service& ioservice):m_hSocket(ioservice)
{
	m_dwSessionID = 0;
	m_pbufPos = m_pRecvBuf;
	m_dwDataLen = 0;
}
CNetSession::~CNetSession()
{

}


//CNetSession* CNetSession::GetInstancePtr()
//{
//	static CNetSession sObject;
//	return &sObject;
//}


boost::asio::ip::tcp::socket& CNetSession::GetSocket()
{
	return m_hSocket;
}

void CNetSession::SetSessionID(UINT32 dwSessionID)
{
	m_dwSessionID = dwSessionID;
}

UINT32		CNetSession::GetSessionID()
{
	return m_dwSessionID;
}

void		CNetSession::SetDataHandler(IDataHandler* pDataHandler)
{
	m_pDataHandler = pDataHandler;
}
IDataHandler* CNetSession::GetDataHandler()
{
	return m_pDataHandler;
}

bool	CNetSession::HandleRecvEvent(UINT32 dwBytes)
{
	m_dwDataLen += dwBytes;

	//1.提取数据
	if (0 == m_dwDataLen)
	{
		return true;
	}

	//2.继续接收没有接收完的
	return DoReceive();
}
void	CNetSession::HandReaddata(const boost::system::error_code& error, UINT32 len)
{
	if (error)
	{
		//Close();
		return;
	}
	HandleRecvEvent(len);
}

bool CNetSession::DoReceive()
{
	m_hSocket.async_read_some(boost::asio::buffer(m_pRecvBuf + m_dwDataLen, RECV_BUF_SIZE - m_dwDataLen), boost::bind(&CNetSession::HandReaddata, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	return true;
}


CNetSessionMrg::CNetSessionMrg()
{

}
CNetSessionMrg::~CNetSessionMrg()
{

}
CNetSessionMrg* CNetSessionMrg::GetInstancePtr()
{
	static CNetSessionMrg sObject;
	return &sObject;
}
bool				CNetSessionMrg::InitConnectionList(UINT32 nMaxCons, boost::asio::io_service& ioservice)
{
	m_NetSessionVector.assign(nMaxCons, nullptr);
	for (UINT32 i = 0; i < nMaxCons; i++)
	{
		std::shared_ptr<CNetSession>CNetSession_ = std::make_shared<CNetSession>(ioservice);
		CNetSession_->SetSessionID(i + 1);
		m_NetSessionVector.push_back(CNetSession_);
	}

	return false;
}
std::shared_ptr<CNetSession> CNetSessionMrg::CreateNetSession()
{
	std::shared_ptr<CNetSession> CNetSession_{};
	if (m_NetSessionVector.size() <= 0)
	{
		return CNetSession_;
	}
	m_GetSessionMutex.lock();
	CNetSession_ = m_NetSessionVector.front();
	m_NetSessionVector.pop_back();
	m_GetSessionMutex.unlock();

	return CNetSession_;
}