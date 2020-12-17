#include "../ServerEngine/PCH.h"
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