#pragma once
#include "PCH.h"
#pragma pack(1)

#define RECV_BUF_SIZE               8192


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

//���ݰ�
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
	UINT32	 CheckCode;		//�������(�Զ���ģ����Ҫ�Ϳͻ���������)
	UINT32   dwMsgID;		//��ϢId
	UINT32   dwSize;		//�����ܳ���(��ͷ+����)
	UINT32   dwPacketNo;	//������� = wCommandID^dwSize+index(ÿ�����Զ���������); ��ԭ��� = pHeader->dwPacketNo - pHeader->wCommandID^pHeader->dwSize;
	UINT64   u64TargetID;	//Ŀ��Id
	UINT32   dwUserData;	//�ͻ����ڸ����е�����£�dwUserData��copyguid�� ���߼���������£� ����Э��䵱�ͻ�������ID
};



struct ServerConfigInfo
{
	UINT32	ServerId;
	UINT16	Prot;
	UINT32	MaxConct;
	std::string	ServerName;
	std::string	Ip;
};



#pragma pack()