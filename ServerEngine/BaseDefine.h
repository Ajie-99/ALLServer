#pragma once
#include "PCH.h"
#pragma pack(1)




static const char* wb_accept = "HTTP/1.1 101 Switching Protocols\r\n" \
"Upgrade:websocket\r\n" \
"Connection: Upgrade\r\n" \
"Sec-WebSocket-Accept: %s\r\n" \
"WebSocket-Location: ws://%s:%d/chat\r\n" \
"WebSocket-Protocol:chat\r\n\r\n";

static const char* migic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";


#define RECV_BUF_SIZE               8192
#define MAX_PKG_SIZE				((1<<16) - 1)

enum
{
	NEW_CONNECTION = 1,
	CLOSE_CONNECTION
};


struct IDataBuffer
{
	virtual bool	AddRef() = 0;
	virtual bool	Release() = 0;
	virtual char* GetData() = 0;
	virtual UINT32	GetTotalLenth() = 0;
	virtual void	SetTotalLenth(UINT32 nPos) = 0;
	virtual UINT32  GetBodyLenth() = 0;
	virtual char* GetBuffer() = 0;
	virtual UINT32	GetBufferSize() = 0;
	virtual UINT32  CopyFrom(IDataBuffer* pSrcBuffer) = 0;
	virtual UINT32  CopyTo(char* pDestBuf, UINT32 dwDestLen) = 0;
};

//数据包
struct NetPacket
{
	NetPacket(UINT32 dwConnID = 0, UINT32 dwMsgID = 0, IDataBuffer* pBuffer = NULL)
	{
		m_dwConnID = dwConnID;
		m_pDataBuffer = pBuffer;
		m_dwMsgID = dwMsgID;
	}
	UINT32       m_dwMsgID;
	UINT32       m_dwConnID;
	IDataBuffer* m_pDataBuffer;
};

struct IDataHandler
{
	//virtual bool OnDataHandle(IDataBuffer* pDataBuffer, UINT32 nConnID) = 0;
	virtual bool OnCloseConnect(UINT32 nConnID) = 0;
	virtual bool OnNewConnect(UINT32 nConnID) = 0;
};

struct IPacketDispatcher
{
	//virtual BOOL DispatchPacket(NetPacket* pNetPacket) = 0;
	virtual bool OnCloseConnect(UINT32 nConnID) = 0;
	virtual bool OnNewConnect(UINT32 nConnID) = 0;
	virtual bool OnSecondTimer() = 0;
};



struct PacketHeader
{
	UINT32	 CheckCode;		//检验代码(自定义的，这个要和客户端商量来)
	UINT32   dwMsgID;		//消息Id
	UINT32   dwSize;		//包的总长度(包头+包体)
	UINT32   dwPacketNo;	//生成序号 = wCommandID^dwSize+index(每个包自动增长索引); 还原序号 = pHeader->dwPacketNo - pHeader->wCommandID^pHeader->dwSize;
	UINT64   u64TargetID;	//目标Id
	UINT32   dwUserData;	//客户端在副本中的情况下，dwUserData是copyguid， 在逻辑服的情况下， 部分协议充当客户端连接ID
};



struct ServerConfigInfo
{
	UINT32	ServerId;
	UINT16	Prot;
	UINT32	MaxConct;
	std::string	ServerName;
	std::string	Ip;
};
/*
* -- 包格式
-- 两字节包长
-- 两字节协议号
-- 两字符字符串长度
-- 字符串内容
*/
struct PackTest
{
	INT16 PackSize;
	INT16 MsgId;
	INT16 MsgStringSize;
};

#pragma pack()