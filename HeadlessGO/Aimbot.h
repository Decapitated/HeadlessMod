#pragma once
#include <iostream>

#include "Minhook/MinHook.h"
#if _WIN64
#pragma comment(lib, "MinHook.x64.lib")
#else
#pragma comment(lib, "MinHook.x86.lib")
#endif

#include "Menu.h"
#include "GameData.h"

namespace Hack {
	class Aimbot
	{
	public:
		Aimbot();
		~Aimbot();

	private:
		void* vt_clientShared[22];
	};
}