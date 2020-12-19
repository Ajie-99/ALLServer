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
		if (0 == CHttpParser::has_client_key)
		{
			http_parser p{};
			http_parser_init(&p, HTTP_REQUEST);

			http_parser_settings s{};
			http_parser_settings_init(&s);
			s.on_header_field = on_header_field;
			s.on_header_value = on_header_value;

			has_client_key = 0;
			http_parser_execute(&p, &s, m_pbufPos, m_dwDataLen);

		}
		if (0 == has_client_key)
		{
			//˵������û�гɹ�
			//1.�Ͽ�����
			//2.�ر�socket
			std::cout << "---����" << m_dwSessionID << "������---" << std::endl;
			return true;
		}
		
		// ��һ��http�����ݸ����ǵ�client,����websocket����
		static char key_migic[256]{};
		const char* migic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		sprintf(key_migic, "%s%s", client_ws_key, migic);

		int sha1_size = 0; // ��ż��ܺ�����ݳ���
		int base64_len = 0;
		char* sha1_content = crypt_sha1((uint8_t*)key_migic, strlen(key_migic), &sha1_size);
		char* b64_str = base64_encode((uint8_t*)sha1_content, sha1_size, &base64_len);

		strncpy(key_migic, b64_str, base64_len);
		key_migic[base64_len] = 0;

		static char accept_buffer[256]{};
		std::string ip = m_hSocket.remote_endpoint().address().to_string();

		UINT16 port = m_hSocket.remote_endpoint().port();
		sprintf(accept_buffer, wb_accept, key_migic, ip.c_str(), port);
		//send(ses->c_sock, accept_buffer, strlen(accept_buffer), 0);
		std::string ss = std::string(accept_buffer);
		IDataBuffer* pwebDataBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(ss.length());
		pwebDataBuffer->SetTotalLenth(ss.length());
		memcpy(pwebDataBuffer->GetBuffer(), ss.c_str(), ss.length());
		lockfree_queue.push(pwebDataBuffer);
		DoSend();




		std::cout <<"---����"<<m_dwSessionID<< "������---" << std::endl;
		if (m_pCurRecvBuffer != nullptr)
		{
			//�����������ʾ���ݻ�û�����꣬Ҫ����
		}

		/*
		*	-- ����ʽ
			-- ���ֽڰ���
			-- ���ֽ�Э���
			-- ���ַ��ַ�������
			-- �ַ�������
		*/
		PackTest * pHeader = (PackTest*)m_pRecvBuf;
		//PacketHeader* pHeader = (PacketHeader*)m_pRecvBuf;
		IDataBuffer* pDataBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(m_dwDataLen);
		memcpy(pDataBuffer->GetBuffer(), m_pbufPos, m_dwDataLen);
		 
		std::string b = pDataBuffer->GetBuffer() + sizeof(PackTest);
		//std::cout << "-------"<< b <<"-----------"<<std::endl;
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