
#include "CommonFunc.h"
#include <windows.h>



void CommonFunc::SetWorkDir()
{
	std::string path = GetCurrentExeDir();
	SetCurrentDirectoryA(path.c_str());
}
std::string CommonFunc::GetWorkDir()
{
	char buf[120]{};
	GetCurrentDirectoryA(120,buf);
	return std::string(buf);
}
std::string CommonFunc::GetCurrentExeDir()
{
	char szPath[1024]{};
	GetModuleFileNameA(NULL, szPath, 1024);
	char* p = strrchr(szPath, '\\');
	* p = 0;
	return std::string(szPath);
}

bool CommonFunc::SpliteString(std::string strSrc, std::string strDelim, std::vector<std::string>& vtStr)
{
	vtStr.clear();
	if (strDelim.empty())
	{
		vtStr.push_back(strSrc);
		return true;
	}

	std::string::iterator subStart, subEnd;
	subStart = strSrc.begin();
	while (true)
	{
		subEnd = std::search(subStart, strSrc.end(), strDelim.begin(), strDelim.end());
		std::string temp(subStart, subEnd);
		if (!temp.empty())
		{
			vtStr.push_back(temp);
		}
		if (subEnd == strSrc.end())
		{
			break;
		}
		subStart = subEnd + strDelim.size();
	}

	return true;
}

INT32 CommonFunc::StringToInt(const char* pStr)
{
	if (pStr == NULL)
	{
		return 0;
	}

	return atoi(pStr);
}

INT64 CommonFunc::StringToInt64(const char* pStr)
{
	if (pStr == NULL)
	{
		return 0;
	}
	return _atoi64(pStr);
}


float  CommonFunc::StringToFloat(const char* pStr)
{
	if (pStr == NULL)
	{
		return 0;
	}

	return (float)atof(pStr);
}

std::string CommonFunc::IntToString(INT32 nValue)
{
	char szValue[64] = { 0 };

	snprintf(szValue, 64, "%d", nValue);

	return std::string(szValue);
}

std::string CommonFunc::FloatToString(float fValue, INT32 nPrecision, bool bRound)
{
	CHAR szValue[64] = { 0 };

	if ((bRound) && (nPrecision > 0) && (nPrecision < 6))
	{
		float fRoundValue = 5;

		for (int i = 0; i < nPrecision + 1; i++)
		{
			fRoundValue *= 0.1f;
		}

		fValue += fRoundValue;
	}

	snprintf(szValue, 64, "%f", fValue);

	CHAR* pChar = strchr(szValue, '.');
	if (pChar == NULL)
	{
		return std::string(szValue);
	}

	*(pChar + nPrecision + 1) = '\0';

	return std::string(szValue);
}