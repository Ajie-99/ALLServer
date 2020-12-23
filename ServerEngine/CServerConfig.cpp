#include "PCH.h"
#include "CServerConfig.h"
#include "../Common/CommonFunc.h"

CServerConfig* CServerConfig::GetInstancePtr()
{
	static CServerConfig sObject;
	return &sObject;
}

CServerConfig::CServerConfig()
{
	SetConfigPath("../ServerData/ServerConfig.json");
	Init();
}

void CServerConfig::SetConfigPath(std::string filePath)
{
	m_FilePath = std::move(filePath);
}


bool CServerConfig::Init()
{
	if (m_FilePath.empty())return false;
	std::ifstream t(m_FilePath);
	std::string str((std::istreambuf_iterator<char>(t)),std::istreambuf_iterator<char>());
	rapidjson::Document document;
	document.Parse(str.c_str());
	{
		rapidjson::Value& DocRoot = document["root"];
		if (DocRoot.IsObject())
		{
			auto DocIter = DocRoot.MemberBegin();
			for (; DocIter != DocRoot.MemberEnd(); ++DocIter)
			{
				rapidjson::Value& V = DocRoot[DocIter->name];
				ServerConfigInfo ConfigInfo{};
				UINT32 ServerId						= V["ServerId"].GetInt();
				ConfigInfo.ServerId					= ServerId;
				std::string ServerName				= V["ServerName"].GetString();
				ConfigInfo.ServerName				= ServerName;
				ConfigInfo.Ip						= V["Ip"].GetString();
				ConfigInfo.Prot						= V["Prot"].GetInt();
				ConfigInfo.MaxConct					= V["MaxConct"].GetInt();
				m_ServerConfigInfo[ServerId]		= ConfigInfo;
				m_strServerConfigInfo[ServerName]	= ConfigInfo;
			}
		}
	}
	return true;
}


ServerConfigInfo CServerConfig::GetServerInfoByServerId(UINT32 nId)const
{
	ServerConfigInfo ConfigInfo{};
	auto Iter = m_ServerConfigInfo.find(nId);
	if (Iter != m_ServerConfigInfo.end())
	{
		ConfigInfo = Iter->second;
	}
	return ConfigInfo;
}


ServerConfigInfo CServerConfig::GetServerInfoByServerName(std::string ServrName)const
{
	ServerConfigInfo ConfigInfo{};
	auto Iter = m_strServerConfigInfo.find(ServrName);
	if (Iter != m_strServerConfigInfo.end())
	{
		ConfigInfo = Iter->second;
	}
	return ConfigInfo;
}