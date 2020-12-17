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
private:
	bool WaitForConnet();
	void HandleAccept(CNetSession* NetSession_, const boost::system::error_code& e);
private:
	 
	IDataHandler*										m_pBufferHandler;
	boost::asio::io_service								m_IoService;
	boost::asio::ip::tcp::acceptor						*m_pAcceptor;
	std::thread											*m_pWorkThread;
};

