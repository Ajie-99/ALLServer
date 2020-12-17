#include "CGameService.h"
#include "../Common/CommonFunc.h"
int main()
{
	CommonFunc::SetWorkDir();

	CGameService::GetInstancePtr()->Init();

	return 0;
}


