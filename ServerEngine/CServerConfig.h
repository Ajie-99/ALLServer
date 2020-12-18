#pragma once
#include "BaseDefine.h"
#include "rapidjson/document.h"

class CServerConfig
{
public:
	static CServerConfig* GetInstancePtr();
	CServerConfig();
	void SetConfigPath(std::string filePath);
	bool Init();

	ServerConfigInfo &GetServerInfoByServerId(UINT32 nId)const;
	ServerConfigInfo	&GetServerInfoByServerName(std::string ServrName)const;
private:
	~CServerConfig() {}
private:
	std::map<UINT32, ServerConfigInfo>m_ServerConfigInfo;
	std::map<std::string, ServerConfigInfo>m_strServerConfigInfo;
	std::string m_FilePath;
};

