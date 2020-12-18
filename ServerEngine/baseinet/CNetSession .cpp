#include "CNetSession .h"

CNetSession::CNetSession(boost::asio::io_service& ioservice) :m_hSocket(ioservice)
{
	memset(m_pRecvBuf, 0, sizeof(m_pRecvBuf));
	m_pDataHandler = nullptr;
	m_dwSessionID = 0;
	m_pbufPos = m_pRecvBuf;
	m_dwDataLen = 0;
	m_pCurRecvBuffer = nullptr;
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
	if (nullptr == pDataHandler)assert(0);
	m_pDataHandler = pDataHandler;
}
IDataHandler* CNetSession::GetDataHandler()
{
	return m_pDataHandler;
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
void		CNetSession::HandWritedata(const boost::system::error_code& error, size_t len)
{
	if (error)return;

	DoSend();
}
bool	CNetSession::HandleRecvEvent(UINT32 dwBytes)
{
	m_dwDataLen += dwBytes;
	if (0 == m_dwDataLen)
	{
		return true;
	}
	//1.��ȡ����
	for (;;)
	{
		std::cout <<"---����"<<m_dwSessionID<< "������---" << std::endl;
		if (m_pCurRecvBuffer != nullptr)
		{
			//�����������ʾ���ݻ�û�����꣬Ҫ����
		}
		PacketHeader* pHeader = (PacketHeader*)m_pRecvBuf;
		IDataBuffer* pDataBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(m_dwDataLen);
		memcpy(pDataBuffer->GetBuffer(), m_pbufPos, m_dwDataLen);

		std::string b = pDataBuffer->GetBuffer() + sizeof(PacketHeader);
		std::cout << "-------"<< b <<"-----------"<<std::endl;
		break;
	}
	//2.��֤����
		//2.1��֤�ɹ��ַ���Ϣ
		//2.2��֤ʧ�ܲ�������,���߶Ͽ�socket
	//3.��������û�н������
	return DoReceive();
}

bool CNetSession::DoReceive()
{
	m_hSocket.async_read_some(boost::asio::buffer(m_pRecvBuf + m_dwDataLen, RECV_BUF_SIZE - m_dwDataLen), boost::bind(&CNetSession::HandReaddata, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	return true;
}


bool CNetSession::DoSend()
{
	int nSendSize = 0;
	IDataBuffer * pBuffer = nullptr;
	IDataBuffer* pSendedBuf = nullptr;
	char buf[1024]{};
	while (lockfree_queue.pop(pBuffer))
	{
		nSendSize += pBuffer->GetTotalLenth();
		pSendedBuf = CBufferAllocator::GetInstancePtr()->AllocDataBuff(RECV_BUF_SIZE);


		pBuffer->CopyTo(buf, pBuffer->GetTotalLenth());
		//δ�����...

	}
	boost::asio::async_write(m_hSocket, boost::asio::buffer(buf, pBuffer->GetBufferSize()), boost::bind(&CNetSession::HandWritedata, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	
	return true;
}

bool CNetSession::SendBuffer(IDataBuffer* pBuff)
{
	return lockfree_queue.push(pBuff);
}


//////////////////////////////////////////////////////////////////////////

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
	//m_NetSessionVector.assign(nMaxCons, nullptr);
	for (UINT32 i = 0; i < nMaxCons; i++)
	{
		CNetSession*CNetSession_ = new CNetSession(ioservice);
		CNetSession_->SetSessionID(i + 1);
		m_NetSessionVector.push_back(CNetSession_);
	}

	return false;
}
CNetSession* CNetSessionMrg::CreateNetSession()
{
	m_GetSessionMutex.lock();
	CNetSession* CNetSession_ = nullptr;
	if (m_NetSessionVector.size() <= 0)
	{
		return nullptr;
	}
	//CNetSession_ = m_NetSessionVector.front();
	//m_NetSessionVector.pop_front();
	CNetSession_ = m_NetSessionVector.back();
	m_OnlineNetSessionVector[CNetSession_->GetSessionID()] = CNetSession_;
	m_NetSessionVector.pop_back();
	m_GetSessionMutex.unlock();

	return CNetSession_;
}

CNetSession* CNetSessionMrg::GetNetSessionByID(UINT32 dwConnID)
{
	if (0 == dwConnID)return nullptr;
	CNetSession* NetSession = nullptr;
	auto Iter = m_OnlineNetSessionVector.find(dwConnID);
	if (Iter != m_OnlineNetSessionVector.end())
	{
		NetSession = Iter->second;
	}
	//CNetSession* NetSession = nullptr;
	//UINT32 dwIndex = dwConnID % m_OnlineNetSessionVector.size();
	//NetSession = m_OnlineNetSessionVector[dwIndex == 0 ? (m_OnlineNetSessionVector.size() - 1) : (dwIndex - 1)];
	if (NetSession->GetSessionID() != dwConnID)
	{
		NetSession = nullptr;
	}
	return NetSession;
}