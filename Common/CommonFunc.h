#pragma once

#include "../ServerEngine/PCH.h"
class CommonFunc
{
public:
	static void SetWorkDir();
	static std::string GetWorkDir();
	static std::string GetCurrentExeDir();
	static bool SpliteString(std::string strSrc, std::string strDelim, std::vector<std::string>& vtStr);
	static INT32 StringToInt(const char* pStr);
	static INT64 StringToInt64(const char* pStr);
	static float  StringToFloat(const char* pStr);
	static std::string IntToString(INT32 nValue);
	static std::string FloatToString(float fValue, INT32 nPrecision = -1, bool bRound = false);
};

