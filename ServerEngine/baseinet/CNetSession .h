#pragma once
#include "../BaseDefine.h"
#include "../Platform.h"
#include "CHttpParser.h"
#include "HttpParameter.h"
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
	void			HandReaddata(const boost::system::error_code& error, size_t len);
	void			HandWritedata(const boost::system::error_code& error, size_t len);
	bool			DoReceive();
	bool			DoSend();
	void			Close();
	bool            SendBuffer(IDataBuffer* pBuff);
public:
	bool			CheckHttpPackkage();
	bool			RetHttpHandlerPackkage();
	bool			ExtractBuffer();
	bool			CheckHeader(char* m_pPacket);
public:
	boost::asio::ip::tcp::socket	m_hSocket;
private:
	INT32							m_dwSessionID;
	IDataBuffer						*m_pCurRecvBuffer;
	UINT32							m_pCurBufferSize;
	char							m_pRecvBuf[RECV_BUF_SIZE];
	char							* m_pBufPos;
	UINT32							m_dwDataLen;//一次性接受到的总包长度
	IDataHandler					*m_pDataHandler;
	bool							m_IsHandler;
	//boost::lockfree::queue<IDataBuffer*> queue;
	boost::lockfree::queue<IDataBuffer*, boost::lockfree::capacity<40000> > lockfree_queue;

	http_parser						m_http_parser;
	http_parser_settings			m_http_parser_settings;
	char							key_migic[256];
	char							accept_buffer[256];
	std::string                     m_RecvBuf;
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