#include "CGameService.h"

int main()
{
	if (CGameService::GetInstancePtr()->Init())
	{
		CGameService::GetInstancePtr()->Run();
	}

	std::cin.get();
	return 0;
}