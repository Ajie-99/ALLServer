#pragma once


class CNetSession;
// ������Ϣͷ����ȡ��������
struct SNetHeader
{
	int size;
	int type;
};
// ������Ϣ�ṹ��
typedef bool (*msg_handle)(const char* pMsg, int n32MsgLength, CNetSession* vthis, int n32MsgID);

// ������Ϣ�ڵ㣬������������==>������Ϣ�����ַ�֮
struct MsgNode
{
	msg_handle mHandle;
	bool mIsInitMsg;
	MsgNode() { mIsInitMsg = false; mHandle = nullptr; }
};

class IMsgHandle
{
public:
	IMsgHandle(int base, int max);
	virtual ~IMsgHandle();
public:
	void RegisterMsgFunc(int type, msg_handle fuc_handle, bool isInitMsg = false);
	void SetUnKnownMsgHandle(msg_handle fuc_handle);
	MsgNode* mNodes;
	msg_handle mUnknownHandle;
	int mMax;
	int mBase;
};

