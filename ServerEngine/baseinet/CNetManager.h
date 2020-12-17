#pragma once



class IDataHandler;
class CNetSession;

class CNetManager
{
public:
	static CNetManager* GetInstancePtr();
	CNetManager();

public:
	bool Start(std::string& strListenIp, UINT32 nPort, UINT32 nMaxConn, IDataHandler* pBufferHandler);
private:
	~CNetManager();
	bool WaitForConnet();
	void HandleAccept(const std::shared_ptr<CNetSession>& NetSession_, const boost::system::error_code& e);
private:
	 
	IDataHandler*										m_pBufferHandler;
	boost::asio::io_service								m_IoService;
	std::shared_ptr<boost::asio::ip::tcp::acceptor>		m_pAcceptor;
	std::shared_ptr<std::thread>						m_pWorkThread;
};

