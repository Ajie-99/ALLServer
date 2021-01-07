#include "CMsgDispacher.h"

#define REGISTER_CALLBACK_IMP(msg) this->registerMessageCallback<rpc::msg>(delegate::from_method<CMsgDispacher, &CMsgDispacher::OnSync##msg>(this))
#define PROCESS_MESSAGE_START(msg)\
    void CMsgDispacher::OnSync##msg(unsigned long lClientId, google::protobuf::Message* m)\
    {\
        rpc::msg* rst = static_cast<rpc::msg*>(m);\
        //Player *player =  GameServer::getInstance()->GetPlayer(lClientId);\
        Account *account =  GameServer::getInstance()->GetAccount(lClientId);\
		//syslog(rst->GetTypeName().c_str());

#define PROCESS_MESSAGE_END }



CMsgDispacher::CMsgDispacher()
{
    REGISTER_CALLBACK_IMP(LoginInfo);
    REGISTER_CALLBACK_IMP(LoginOut);
}

PROCESS_MESSAGE_START(LoginInfo)
std::cout << "---LoginInfo---" << std::endl;
PROCESS_MESSAGE_END;


PROCESS_MESSAGE_START(LoginOut)
std::cout << "---LoginOut---" << std::endl;
PROCESS_MESSAGE_END;
