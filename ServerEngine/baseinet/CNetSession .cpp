#include "CNetSession .h"


CNetSession::CNetSession(boost::asio::io_service& ioservice) :m_hSocket(ioservice)
{
	memset(m_pRecvBuf, 0, sizeof(m_pRecvBuf));
	m_pDataHandler = nullptr;
	m_dwSessionID = -1;
	m_pBufPos = m_pRecvBuf;
	m_dwDataLen = 0;
	m_pCurRecvBuffer = nullptr;
	m_pCurBufferSize = 0;
	memset(&m_http_parser, 0, sizeof(m_http_parser));
	memset(&m_http_parser_settings, 0, sizeof(m_http_parser_settings));
	memset(key_migic, 0, sizeof(key_migic));
	m_IsHandler = false;
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


void	CNetSession::HandReaddata(const boost::system::error_code& error, size_t len)
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

bool	CNetSession::CheckHttpPackkage()
{
	CHttpParser::has_client_key = 0;
	memset(&m_http_parser, 0, sizeof(m_http_parser));
	memset(&m_http_parser_settings, 0, sizeof(m_http_parser_settings));
	http_parser_init(&m_http_parser, HTTP_REQUEST);
	http_parser_settings_init(&m_http_parser_settings);
	m_http_parser_settings.on_header_field = CHttpParser::on_header_field;
	m_http_parser_settings.on_header_value = CHttpParser::on_header_value;
	http_parser_execute(&m_http_parser, &m_http_parser_settings, m_pBufPos, m_dwDataLen);

	return 	CHttpParser::has_client_key == 0? false : true;
}
// 回一个http的数据给我们的client,建立websocket链接
bool		CNetSession::RetHttpHandlerPackkage()
{
	memset(key_migic, 0, sizeof(key_migic));
	sprintf(key_migic, "%s%s", CHttpParser::client_ws_key, migic);

	int sha1_size = 0; // 存放加密后的数据长度
	int base64_len = 0;
	char* sha1_content = crypt_sha1((uint8_t*)key_migic, strlen(key_migic), &sha1_size);
	char* b64_str = base64_encode((uint8_t*)sha1_content, sha1_size, &base64_len);
	strncpy(key_migic, b64_str, base64_len);
	key_migic[base64_len] = 0;
	std::string ip = m_hSocket.remote_endpoint().address().to_string();
	UINT16 port = m_hSocket.remote_endpoint().port();
	memset(accept_buffer, 0, sizeof(accept_buffer));
	sprintf(accept_buffer, wb_accept, key_migic, ip.c_str(), port);
	//send(ses->c_sock, accept_buffer, strlen(accept_buffer), 0);
	std::string buffer = std::string(accept_buffer);
	INT32 nLen = buffer.length();
	IDataBuffer* pwebDataBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(nLen);
	pwebDataBuffer->SetTotalLenth(nLen);
	memcpy(pwebDataBuffer->GetBuffer(), buffer.c_str(), nLen);
	lockfree_queue.push(pwebDataBuffer);
	return DoSend();
}


UINT32						m_nCheckNo = 0;
#define CODE_VALUE 0x88
bool CNetSession::CheckHeader(char* m_pPacket)
{
	/*
	1.首先验证包的验证吗
	2.包的长度
	3.包的序号
	*/
	PacketHeader* pHeader = (PacketHeader*)m_pBufPos;
	if (pHeader->CheckCode != CODE_VALUE)
	{
		return false;
	}

	if (pHeader->dwSize > 1024 * 1024)
	{
		return false;
	}

	if (pHeader->dwMsgID > 4999999)
	{
		return false;
	}

	if (m_nCheckNo == 0)
	{
		m_nCheckNo = pHeader->dwPacketNo - (pHeader->dwMsgID ^ pHeader->dwSize) + 1;
		return true;
	}

	if (pHeader->dwPacketNo == (pHeader->dwMsgID ^ pHeader->dwSize) + m_nCheckNo)
	{
		m_nCheckNo += 1;
		return true;
	}

	return false;
}

char* long_pkg = nullptr;

bool CNetSession::ExtractBuffer()
{
	if (m_dwDataLen == 0)return true;
	while (true)
	{
		int pkg_size = 0;
		int header_size = 0;
		CHttpParser::recv_ws_header((unsigned char*)m_pBufPos, m_dwDataLen, &pkg_size, &header_size);
		UINT32 dwPacketSize = pkg_size;

		if (m_pCurRecvBuffer != NULL)
		{
			if ((m_pCurRecvBuffer->GetTotalLenth() + m_dwDataLen) < m_pCurBufferSize)
			{
				memcpy(m_pCurRecvBuffer->GetBuffer() + m_pCurRecvBuffer->GetTotalLenth(), m_pBufPos, m_dwDataLen);
				m_pBufPos = m_pRecvBuf;
				m_pCurRecvBuffer->SetTotalLenth(m_pCurRecvBuffer->GetTotalLenth() + m_dwDataLen);
				m_dwDataLen = 0;
				break;
			}
			else
			{
				memcpy(m_pCurRecvBuffer->GetBuffer() + m_pCurRecvBuffer->GetTotalLenth(), m_pBufPos, m_pCurBufferSize - m_pCurRecvBuffer->GetTotalLenth());
				m_dwDataLen -= m_pCurBufferSize - m_pCurRecvBuffer->GetTotalLenth();
				m_pBufPos += m_pCurBufferSize - m_pCurRecvBuffer->GetTotalLenth();
				m_pCurRecvBuffer->SetTotalLenth(m_pCurBufferSize);
				m_pDataHandler->OnDataHandle(m_pCurRecvBuffer, header_size,GetSessionID());
				m_pCurRecvBuffer = NULL;
			}
		}

		if (dwPacketSize >= MAX_PKG_SIZE)
		{
			Close();
			break;
		}

		if ((dwPacketSize > m_dwDataLen) && (dwPacketSize < RECV_BUF_SIZE))
		{
			break;
		}


		if (dwPacketSize <= m_dwDataLen)
		{
			IDataBuffer* pDataBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(dwPacketSize);
			memcpy(pDataBuffer->GetBuffer(), m_pBufPos, dwPacketSize);
			m_dwDataLen -= dwPacketSize;
			m_pBufPos += dwPacketSize;
			pDataBuffer->SetTotalLenth(dwPacketSize);
			m_pDataHandler->OnDataHandle(pDataBuffer, header_size,GetSessionID());
			break;
		}
		else
		{
			IDataBuffer* pDataBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(dwPacketSize);
			memcpy(pDataBuffer->GetBuffer(), m_pBufPos, m_dwDataLen);
			pDataBuffer->SetTotalLenth(m_dwDataLen);
			m_dwDataLen = 0;
			m_pBufPos = m_pRecvBuf;
			m_pCurRecvBuffer = pDataBuffer;
			m_pCurBufferSize = dwPacketSize;
		}
	}

	if (m_dwDataLen > 0)
	{
		memmove(m_pRecvBuf, m_pBufPos, m_dwDataLen);
	}

	m_pBufPos = m_pRecvBuf;

	return true;
}

bool	CNetSession::HandleRecvEvent(UINT32 dwBytes)
{
	m_dwDataLen += dwBytes;
	if (0 == m_dwDataLen)return true;
	//1.提取数据
	if (!m_IsHandler)
	{
		if (!CheckHttpPackkage())
		{
			//说明握手没有成功
			//关闭socket
			Close();
			return true;
		}
		if (!RetHttpHandlerPackkage())
		{
			return true;
		}
		m_dwDataLen = 0;//要不握手包的数据清零
		m_IsHandler = true;
	}
	else
	{
		ExtractBuffer();
		//DelHttpPackage(dwBytes);
	}

	//2.验证数据
		//2.1验证成功分发消息
		//2.2验证失败不做处理,或者断开socket
	//3.继续接收
	return DoReceive();
}

bool CNetSession::DoReceive()
{
	m_hSocket.async_read_some(boost::asio::buffer(m_pRecvBuf+ m_dwDataLen, RECV_BUF_SIZE - m_dwDataLen), boost::bind(&CNetSession::HandReaddata, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
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
		//未完待续...
		std::cout <<"返回Http :"<< std::endl <<buf << std::endl;
			//这里发送要做个缓存，上pBuffer要pBuffer->Release();
		boost::asio::async_write(m_hSocket, boost::asio::buffer(buf, pBuffer->GetBufferSize()), boost::bind(&CNetSession::HandWritedata, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	
	return true;
}

bool CNetSession::SendBuffer(IDataBuffer* pBuff)
{
	return lockfree_queue.push(pBuff);
}

void		CNetSession::Close()
{
	m_hSocket.shutdown(boost::asio::socket_base::shutdown_receive);
	m_hSocket.shutdown(boost::asio::socket_base::shutdown_send);
	m_hSocket.close();
	if (m_pDataHandler)
	{
		m_pDataHandler->OnCloseConnect(GetSessionID());
	}
	m_IsHandler = false;
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