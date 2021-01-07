#include "CGameService.h"


#include "CMsgDispacher.h"
int main()
{

	CMsgDispacher _CMsgDispacher;

	rpc::LoginInfo l;
	l.set_name("123");
	l.set_pwd("123456");
	google::protobuf::Message* pMsg = (google::protobuf::Message*)&l;

	int msgBufferLength = pMsg->ByteSizeLong();

	char* msgBuffer = new char[msgBufferLength];
	memset(msgBuffer, 0, msgBufferLength);
	pMsg->SerializeToArray(msgBuffer, msgBufferLength);

	rpc::Request request;
	request.set_method(pMsg->GetTypeName().c_str());
	request.set_serialized_request(msgBuffer, msgBufferLength);
	delete[]msgBuffer;
	int reqBufferLength = request.ByteSizeLong() + 2;
	char* reqBuffer = new char[reqBufferLength];
	request.SerializeToArray(reqBuffer + 2, reqBufferLength - 2);
	_CMsgDispacher.OnceDispatch(1, reqBuffer + 2, reqBufferLength - 2);
	delete[]reqBuffer;

	if (CGameService::GetInstancePtr()->Init())
	{
		CGameService::GetInstancePtr()->Run();
	}
	std::cin.get();
	return 0;
}