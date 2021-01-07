#include "../ServerEngine/baseinet/CServiceBase.h"
#include "../ServerEngine/CServerConfig.h"
#include "../Common/CommonFunc.h"

using boost::asio::ip::tcp;
using boost::asio::ip::address;

class client : public boost::enable_shared_from_this<client> 
{
public:
	client(boost::asio::io_service& io_service, tcp::endpoint& endpoint)
		: io_service_(io_service), socket_(io_service), endpoint_(endpoint)
	{

	}

	void start() 
	{
		socket_.async_connect(endpoint_,
			boost::bind(&client::handle_connect,
				shared_from_this(),
				boost::asio::placeholders::error));
	}

private:
	void handle_connect(const boost::system::error_code& error)
	{
		if (error) {
			if (error.value() != boost::system::errc::operation_canceled)
			{
				std::cerr << boost::system::system_error(error).what() << std::endl;
			}

			socket_.close();
			return;
		}

		static tcp::no_delay option(true);
		socket_.set_option(option);

		///
		std::string buffer = "";

		IDataBuffer* pDataBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(buffer.length() + sizeof(PacketHeader));

		std::cout << buffer.length() << std::endl;
		PacketHeader* pHeader = (PacketHeader*)pDataBuffer->GetBuffer();
		pHeader->CheckCode = 0x88;
		pHeader->dwMsgID = 1;
		pHeader->dwPacketNo = 0;
		pHeader->dwSize = buffer.length() + sizeof(PacketHeader);
		pHeader->dwUserData = 0;
		pHeader->u64TargetID = 1;

		memcpy(pDataBuffer->GetBuffer() + sizeof(PacketHeader), buffer.c_str(), buffer.length());
		///
		strcpy_s(buf, pDataBuffer->GetBuffer());
		
		

		boost::asio::async_write(socket_,
			boost::asio::buffer(pDataBuffer->GetBuffer(), pDataBuffer->GetBufferSize()),
			boost::bind(&client::handle_write,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_write(const boost::system::error_code& error, size_t bytes_transferred) 
	{
		memset(buf, sizeof(buf), 0);
		boost::asio::async_read_until(socket_, sbuf, "\n",
			boost::bind(&client::handle_read,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_read(const boost::system::error_code& error, size_t bytes_transferred) 
	{
		std::cout << buf << std::endl;
	}

private:
	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	tcp::endpoint& endpoint_;
	char buf[1024*8];
	boost::asio::streambuf sbuf;
};


typedef boost::shared_ptr<client> client_ptr;

int main()
{
	CommonFunc::SetWorkDir();

	boost::asio::io_service io_service;
	//tcp::endpoint endpoint(address::from_string("127.0.0.1"), 28880);
	tcp::endpoint endpoint(address::from_string("127.0.0.1"), 28880);
	client_ptr new_session(new client(io_service, endpoint));
	new_session->start();
	io_service.run();


	std::cin.get();
	return 0;
}
