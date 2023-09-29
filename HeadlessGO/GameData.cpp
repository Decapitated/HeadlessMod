#include "GameData.h"
using Hack::GameData;
using Hack::Menu;

DWORD WINAPI DataThread(GameData* gameData);

GameData::GameData() {
	dataThreadHandle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)DataThread, this, 0, nullptr);
}

GameData::~GameData() {
	Stop();
	WaitForSingleObject(dataThreadHandle, INFINITE);
	CloseHandle(dataThreadHandle);
}

char* GameData::GetName(int entityIndex)
{
	player_info_t pinfo;
	SourceInterfaces::pEngine->GetPlayerInfo(entityIndex, &pinfo);
	return pinfo.Name;
}

DWORD WINAPI DataThread(GameData* gameData) {
	while (!gameData->ShouldStop())
	{
		std::cout << "Getting data." << std::endl;

		Hack::GData tempData{ SourceInterfaces::pEngine->GetLocalPlayer(), {} };
		int maxEntities = SourceInterfaces::pEntityList->GetHighestEntityIndex();
		for (int i = 0; i <= maxEntities; i++)
		{
			IClientEntity* currentEntity = SourceInterfaces::pEntityList->GetClientEntity(i);
			if (!SDK_Utilities::IsValid(currentEntity, nullptr, Menu::bShowNPC))
			{
				continue;
			}
			Hack::Player tempPlayer{
				i,
				GameData::GetName(i),
			};

			tempData.players.insert({i, tempPlayer});
		}
		gameData->SetData(&tempData);
		Sleep(2000); // Sleep since we don't do much in this thread.
	}
	std::cout << "End Data Thread." << std::endl;
	return 0;
}