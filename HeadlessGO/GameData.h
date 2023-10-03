#pragma once
#include <iostream> // For console output.
#include <unordered_map>
#include <mutex>
#include <vector>
#include <string>
#include "SDK/SDK_Utilities.h"
#include "Menu.h"

namespace Hack {
	struct Player {
		int entityIndex = -1;
		int team = -1;
		std::string name;
		Vector originPos;
		bool hasHead = false;
		Vector headPos;
		std::vector<std::shared_ptr<Vector[]>> segments;
		float distToCrosshair = -1;
	};
	typedef std::unordered_map<int, Player> PlayerList;
	struct GData {
		RECT screenSize;
		int localPlayerIndex = -1;
		PlayerList players;
		int closestToCrosshair = -1;
	};

	class GameData {
	private:
		bool shouldStop = false;
		HANDLE dataThreadHandle;

	public:
		static std::mutex dataMutex;
		static std::shared_ptr<GData> data;
		GameData();
		~GameData();

		void Stop() {
			shouldStop = true;
		}

		bool ShouldStop() {
			return shouldStop;
		}

		static Player GetPlayer(int entityIndex);
		static std::string GetName(int entityIndex);
		static std::shared_ptr<Vector[]> GetSegment(IClientEntity* target, std::string bone_1, std::string bone_2);
		static std::vector<std::shared_ptr<Vector[]>> GetSegments(IClientEntity* target);
		static float GetDistanceToCrosshair(IClientEntity* target, Vector targetPos);
	};
}