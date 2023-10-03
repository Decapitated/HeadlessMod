#include "GameData.h"
using Hack::GameData;
using Hack::Menu;

DWORD WINAPI DataThread(GameData* gameData);

std::mutex GameData::dataMutex;
std::shared_ptr<Hack::GData> GameData::data(nullptr);

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
	auto origin = entity->GetOrigin();
	int headIndex = SDK_Utilities::GrabBone(entity, SDK_Utilities::getBoneArray()[13]); // Returns -1 when not found.
	Vector headPos = (headIndex == -1 ? origin : SDK_Utilities::GetBonePos(entity, headIndex));
	return Hack::Player{
		entityIndex,
		entity->GetTeam(),
		GameData::GetName(entityIndex),
		origin,
		headIndex != -1,
		headPos,
		GameData::GetSegments(entity),
		GameData::GetDistanceToCrosshair(entity, headPos)
	};
}

std::string GameData::GetName(int entityIndex)
{
	player_info_t pinfo;
	SourceInterfaces::pEngine->GetPlayerInfo(entityIndex, &pinfo);
	return pinfo.Name;
}

std::shared_ptr<Vector[]> GameData::GetSegment(IClientEntity* target, std::string bone_1, std::string bone_2)
{
	int bone_a = SDK_Utilities::GrabBone(target, bone_1.c_str());
	int bone_b = SDK_Utilities::GrabBone(target, bone_2.c_str());
	if (bone_a != -1 && bone_b != -1)
	{
		Vector bonePos_a = SDK_Utilities::GetBonePos(target, bone_a);
		Vector bonePos_b = SDK_Utilities::GetBonePos(target, bone_b);
		Vector w_bonePos_a, w_bonePos_b;
		if (SDKMath::WorldToScreen(bonePos_a, w_bonePos_a) && SDKMath::WorldToScreen(bonePos_b, w_bonePos_b))
		{
			std::shared_ptr<Vector[]> segment(new Vector[2]);
			segment[0] = w_bonePos_a;
			segment[1] = w_bonePos_b;
			return segment;
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<Vector[]>> GameData::GetSegments(IClientEntity* target)
{
	return {};
	std::vector<std::shared_ptr<Vector[]>> segments
	{
		//Right Leg
		GetSegment(target, SDK_Utilities::getBoneArray()[21], SDK_Utilities::getBoneArray()[20]), // R Toe -> R Foot
		GetSegment(target, SDK_Utilities::getBoneArray()[20], SDK_Utilities::getBoneArray()[19]), // R Foot -> R Calf
		GetSegment(target, SDK_Utilities::getBoneArray()[19], SDK_Utilities::getBoneArray()[18]), // R Calf -> R Thigh
		GetSegment(target, SDK_Utilities::getBoneArray()[18], SDK_Utilities::getBoneArray()[0]),  // R Thigh -> Pelvis
		//Left Leg
		GetSegment(target, SDK_Utilities::getBoneArray()[17], SDK_Utilities::getBoneArray()[16]), // L Toe -> L Foot
		GetSegment(target, SDK_Utilities::getBoneArray()[16], SDK_Utilities::getBoneArray()[15]), // L Foot -> L Calf
		GetSegment(target, SDK_Utilities::getBoneArray()[15], SDK_Utilities::getBoneArray()[14]), // L Calf -> L Thigh
		GetSegment(target, SDK_Utilities::getBoneArray()[14], SDK_Utilities::getBoneArray()[0]),  // L Thigh -> Pelvis
		//Spine
		GetSegment(target, SDK_Utilities::getBoneArray()[0], SDK_Utilities::getBoneArray()[3]),   // Pelvis -> Spine 2
		//Right Arm
		GetSegment(target, SDK_Utilities::getBoneArray()[11], SDK_Utilities::getBoneArray()[10]), // R Hand -> R Forearm
		GetSegment(target, SDK_Utilities::getBoneArray()[10], SDK_Utilities::getBoneArray()[9]),  // R Forearm -> R Upperarm
		GetSegment(target, SDK_Utilities::getBoneArray()[9], SDK_Utilities::getBoneArray()[3]),   // R Upperarm -> Spine 2
		GetSegment(target, SDK_Utilities::getBoneArray()[9], SDK_Utilities::getBoneArray()[13]),  // R Upperarm -> Head
		//Left Arm
		GetSegment(target, SDK_Utilities::getBoneArray()[7], SDK_Utilities::getBoneArray()[6]),   // L Hand -> L Forearm
		GetSegment(target, SDK_Utilities::getBoneArray()[6], SDK_Utilities::getBoneArray()[5]),   // L Forearm -> L Upperarm
		GetSegment(target, SDK_Utilities::getBoneArray()[5], SDK_Utilities::getBoneArray()[3]),   // L Upperarm -> Spine 2
		GetSegment(target, SDK_Utilities::getBoneArray()[5], SDK_Utilities::getBoneArray()[13])   // L Upperarm -> Head
	};
	
	return segments;
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
		if (SourceInterfaces::pEngine->IsInGame() &&
			SourceInterfaces::pEngine->IsConnected() &&
			!SourceInterfaces::pEngine->IsDrawingLoadingImage())
		{
			// Initialize updated data object.
			auto tempData = std::make_shared<Hack::GData>();
			tempData->screenSize = SDKMath::GetViewport();
			tempData->localPlayerIndex = SourceInterfaces::pEngine->GetLocalPlayer();
			tempData->players = Hack::PlayerList{};

			// Variables for finding closest to Crosshair.
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

				tempData->players.insert({ index, tempPlayer });
			}

			if (closestToCrosshair != -1)
				tempData->closestToCrosshair = closestToCrosshair;

			{
				const std::lock_guard<std::mutex> lock(GameData::dataMutex);
				GameData::data = tempData;
			}
		}
		Sleep(100); // Sleep since we don't do much in this thread.
	}
	std::cout << "End Data Thread." << std::endl;
	return 0;
}