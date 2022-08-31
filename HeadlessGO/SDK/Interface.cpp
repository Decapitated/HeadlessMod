#include "Interface.h"

IClientEntityList* SourceInterfaces::pEntityList;
IEngineClient* SourceInterfaces::pEngine;
IVModelInfo* SourceInterfaces::pModelInfo;
CHLClient* SourceInterfaces::pClient;
IPanel* SourceInterfaces::pPanel;
ISurface* SourceInterfaces::pSurface;
CInput* SourceInterfaces::pInput;
IEngineTrace* SourceInterfaces::pEngineTrace;

bool CreateInterface::Intitialize() {
	auto clientFactory = GetModuleFactory(L"client.dll");
	auto engineFactory = GetModuleFactory(L"engine.dll");
	auto vGUIFactory = GetModuleFactory(L"vgui2.dll");
	auto vGUIMatsurface = GetModuleFactory(L"vguimatsurface.dll");
	auto inputFactory = GetModuleFactory(L"inputsystem.dll");

	SourceInterfaces::pEntityList = GetInterface<IClientEntityList*>(clientFactory, "VClientEntityList003");
	if (SourceInterfaces::pEntityList == nullptr) return false;
	SourceInterfaces::pEngine = GetInterface<IEngineClient*>(engineFactory, "VEngineClient015");
	if (SourceInterfaces::pEngine == nullptr) return false;
	SourceInterfaces::pModelInfo = GetInterface<IVModelInfo*>(engineFactory, "VModelInfoClient006");
	if (SourceInterfaces::pModelInfo == nullptr) return false;
	SourceInterfaces::pClient = GetInterface<CHLClient*>(clientFactory, "VClient017");
	if (SourceInterfaces::pClient == nullptr) return false;
	SourceInterfaces::pPanel = GetInterface<IPanel*>(vGUIFactory, "VGUI_Panel009");
	if (SourceInterfaces::pPanel == nullptr) return false;
	SourceInterfaces::pSurface = GetInterface<ISurface*>(vGUIMatsurface, "VGUI_Surface030");
	if (SourceInterfaces::pSurface == nullptr) return false;
	SourceInterfaces::pInput = GetInterface<CInput*>(inputFactory, "InputSystemVersion001");
	if (SourceInterfaces::pInput == nullptr) return false;
	SourceInterfaces::pEngineTrace = GetInterface<IEngineTrace*>(engineFactory, "EngineTraceClient003");
	if (SourceInterfaces::pEngineTrace == nullptr) return false;
	return true;
}