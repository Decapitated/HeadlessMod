#include "GameData.h"
using Hack::GameData;
using Hack::Menu;

DWORD WINAPI DataThread(GameData* gameData);

std::mutex GameData::dataMutex;
shared_ptr<Hack::GData> GameData::data;

GameData::GameData() {
	dataThreadHandle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)DataThread, this, 0, nullptr);
}

GameData::~GameData() {
	Stop();
	WaitForSingleObject(dataThreadHandle, INFINITE);
	CloseHandle(dataThreadHandle);
}

Hack::Player GameData::GetPlayer(int entityIndex)
{
	IClientEntity* entity = (IClientEntity*)SourceInterfaces::pEntityList->GetClientEntity(entityIndex);
	auto name = GameData::GetName(entityIndex);
	auto origin = entity->GetOrigin();
	int headIndex = SDK_Utilities::GrabBone(entity, SDK_Utilities::getBoneArray()[13]); // Returns -1 when not found.
	Vector headPos = (headIndex == -1 ? origin : SDK_Utilities::GetBonePos(entity, headIndex));
	return Hack::Player{
		entityIndex,
		name,
		origin,
		headIndex != -1,
		headPos,
		GameData::GetDistanceToCrosshair(entity, headPos)
	};
}

string GameData::GetName(int entityIndex)
{
	player_info_t pinfo;
	SourceInterfaces::pEngine->GetPlayerInfo(entityIndex, &pinfo);
	return pinfo.Name;
}

float GameData::GetDistanceToCrosshair(IClientEntity* target, Vector targetPos)
{
	Vector crosshair(GameData::data->screenSize.right / 2.f, GameData::data->screenSize.bottom / 2.f, 0.0);
	Vector targetScreen;
	if (SDKMath::WorldToScreen(targetPos, targetScreen) && SDK_Utilities::GetVisible(target))
	{
		return crosshair.Dist(targetScreen);
	}
	return -1.0f;
}

DWORD WINAPI DataThread(GameData* gameData) {
	while (!gameData->ShouldStop())
	{
		// Initialize updated data object.
		auto tempData = make_shared<Hack::GData>();
		tempData->screenSize = SDKMath::GetViewport();
		tempData->localPlayerIndex = SourceInterfaces::pEngine->GetLocalPlayer();
		tempData->players = make_shared<Hack::PlayerList>();

		int closestToCrosshair = -1;
		float distance = INFINITY;

		int maxEntities = SourceInterfaces::pEntityList->GetHighestEntityIndex();
		for (int index = 0; index <= maxEntities; index++)
		{
			IClientEntity* currentEntity = SourceInterfaces::pEntityList->GetClientEntity(index);
			if (!SDK_Utilities::IsValid(currentEntity, nullptr, Menu::bShowNPC))
				continue;

			Hack::Player tempPlayer = GameData::GetPlayer(index);

			if (tempPlayer.distToCrosshair != -1 && tempPlayer.distToCrosshair < distance)
			{
				closestToCrosshair = index;
				distance = tempPlayer.distToCrosshair;
			}

			tempData->players->insert({index, tempPlayer});
		}

		if (closestToCrosshair != -1)
			tempData->closestToCrosshair = closestToCrosshair;

		{
			const std::lock_guard<std::mutex> lock(GameData::dataMutex);
			GameData::data = tempData;
		}

		Sleep(100); // Sleep since we don't do much in this thread.
	}
	std::cout << "End Data Thread." << std::endl;
	return 0;
}