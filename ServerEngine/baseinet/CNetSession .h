#pragma once

#define RECV_BUF_SIZE               8192



class IDataHandler;

class CNetSession
{
public:
	CNetSession(boost::asio::io_service& ioservice);
	~CNetSession();



	boost::asio::ip::tcp::socket& GetSocket();

	void			SetSessionID(UINT32 dwSessionID);
	UINT32			GetSessionID();
	void			SetDataHandler(IDataHandler* pDataHandler);
	IDataHandler*	GetDataHandler();
	bool			HandleRecvEvent(UINT32 dwBytes);
	void			HandReaddata(const boost::system::error_code& error, UINT32 len);
	bool			DoReceive();
public:
	boost::asio::ip::tcp::socket	m_hSocket;
private:
	UINT32							m_dwSessionID;
	CHAR							m_pRecvBuf[RECV_BUF_SIZE];
	CHAR							*m_pbufPos;
	UINT32							m_dwDataLen;
	IDataHandler					*m_pDataHandler;
};


class CNetSessionMrg
{
public:
	CNetSessionMrg();
	~CNetSessionMrg();
	static CNetSessionMrg* GetInstancePtr();
	bool				InitConnectionList(UINT32 nMaxCons, boost::asio::io_service& ioservice);
	std::shared_ptr<CNetSession> CreateNetSession();
private:
	std::mutex			m_GetSessionMutex;
	std::vector<std::shared_ptr<CNetSession>>m_NetSessionVector;
};