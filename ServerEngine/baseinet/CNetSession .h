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
	void			HandWritedata(const boost::system::error_code& error, size_t len);
	bool			DoReceive();
	bool			DoSend();
	bool            SendBuffer(IDataBuffer* pBuff);

public:
	boost::asio::ip::tcp::socket	m_hSocket;
private:
	UINT32							m_dwSessionID;
	void							*m_pCurRecvBuffer;
	CHAR							m_pRecvBuf[RECV_BUF_SIZE];
	CHAR							*m_pbufPos;
	UINT32							m_dwDataLen;
	IDataHandler					*m_pDataHandler;
	//boost::lockfree::queue<IDataBuffer*> queue;
	boost::lockfree::queue<IDataBuffer*, boost::lockfree::capacity<40000> > lockfree_queue;
};


class CNetSessionMrg
{
public:
	CNetSessionMrg();
	~CNetSessionMrg();
	static CNetSessionMrg*	GetInstancePtr();
	bool					InitConnectionList(UINT32 nMaxCons, boost::asio::io_service& ioservice);
	CNetSession*			CreateNetSession();
	CNetSession*			GetNetSessionByID(UINT32 dwConnID);
private:
	std::mutex			m_GetSessionMutex;
	std::vector<CNetSession*>m_NetSessionVector;
	std::map<UINT32,CNetSession*>m_OnlineNetSessionVector;
};