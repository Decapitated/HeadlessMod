#pragma once
#include <Windows.h>

#include "SDK.h"

typedef void* (__cdecl* tCreateInterface)(const char* name, int* returnCode);

class CreateInterface {
private:
	tCreateInterface GetModuleFactory(LPCWSTR dll) {
		return reinterpret_cast<tCreateInterface>(
			GetProcAddress(GetModuleHandle(dll), "CreateInterface"));
	}

	template<typename T>
	T GetInterface(tCreateInterface fn, const char* interfaceName) {
		auto inter = reinterpret_cast<T>(fn(interfaceName, nullptr));
		if (!inter) return nullptr;
		return inter;
	}
public:
	bool Intitialize();
};

struct SourceInterfaces {
	static IClientEntityList* pEntityList;
	static IEngineClient* pEngine;
	static IVModelInfo* pModelInfo;
	static CHLClient* pClient;
	static IPanel* pPanel;
	static ISurface* pSurface;
	static CInput* pInput;
	static IEngineTrace* pEngineTrace;
};