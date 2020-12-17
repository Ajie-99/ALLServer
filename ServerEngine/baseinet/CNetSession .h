#pragma once
#include "../BaseDefine.h"
#include "../Platform.h"


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
	void							*m_pCurRecvBuffer;
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
	CNetSession* CreateNetSession();
private:
	std::mutex			m_GetSessionMutex;
	std::list<CNetSession*>m_NetSessionVector;
};