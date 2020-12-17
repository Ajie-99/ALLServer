#include "../ServerEngine/PCH.h"
#include "CommonConvert.h"


std::string CommonConvert::IntToString(INT32 nValue)
{
	char szValue[64] = { 0 };

	snprintf(szValue, 64, "%d", nValue);

	return std::string(szValue);
}