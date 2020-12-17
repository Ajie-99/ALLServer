#include "../PCH.h"
#include "../Platform.h"

#include "CNetManager.h"
#include "CNetSession .h"
#include "../BaseDefine.h"


CNetManager::CNetManager()
{
	m_pBufferHandler = nullptr;
}
CNetManager::~CNetManager()
{

}

CNetManager* CNetManager::GetInstancePtr()
{
	static CNetManager sObject;
	return &sObject;
 }


bool CNetManager::Start(std::string& strListenIp, UINT32 nPort, UINT32 nMaxConn, IDataHandler* pBufferHandler)
{
	m_pBufferHandler = pBufferHandler;

	CNetSessionMrg::GetInstancePtr()->InitConnectionList(nMaxConn, m_IoService);

	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(strListenIp), nPort);
	m_pAcceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(m_IoService, ep, true);

	WaitForConnet();

	m_pWorkThread = std::make_shared<std::thread>(boost::bind(&boost::asio::io_service::run, &m_IoService));
	return false;
}


void CNetManager::HandleAccept(const std::shared_ptr<CNetSession>& NetSession_, const boost::system::error_code& e)
{
	if (e)
	{
		//这里是监听出错，需要处理．
		//NetSession_->Close();
		return;
	}
	m_pBufferHandler->OnNewConnect(NetSession_->GetSessionID());
	NetSession_->DoReceive();

	WaitForConnet();
}

bool CNetManager::WaitForConnet()
{
	std::shared_ptr<CNetSession> CNetSession_ = CNetSessionMrg::GetInstancePtr()->CreateNetSession();
	CNetSession_->SetDataHandler(m_pBufferHandler);

	m_pAcceptor->async_accept(CNetSession_->GetSocket(), boost::bind(&CNetManager::HandleAccept, this, CNetSession_, boost::asio::placeholders::error));
	return false;
}