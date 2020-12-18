#pragma once
#include "CNetSession .h"
class CNetManager
{
public:
	static CNetManager* GetInstancePtr();
	CNetManager();
	~CNetManager();
public:
	bool Start(std::string& strListenIp, UINT32 nPort, UINT32 nMaxConn, IDataHandler* pBufferHandler);
	CNetSession* ConnectTo_Async(std::string strIpAddr, UINT16 sPort);
private:
	bool			WaitForConnet();
	void			HandleAccept(CNetSession* NetSession_, const boost::system::error_code& e);

	void			HandleConnect(CNetSession* pNetSession, const boost::system::error_code& e);



public:
	bool			SendMessageData(UINT32 dwConnID, UINT32 dwMsgID, UINT64 u64TargetID, UINT32 dwUserData, const char* pData, UINT32 dwLen);
private:
	IDataHandler*										m_pBufferHandler;
	boost::asio::io_service								m_IoService;
	boost::asio::ip::tcp::acceptor						*m_pAcceptor;
	std::thread											*m_pWorkThread;
};

