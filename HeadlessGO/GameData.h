#pragma once
#include <iostream> // For console output.
#include <unordered_map>
#include "SDK/SDK_Utilities.h"
#include "Menu.h"
#include <mutex>

using namespace std;

namespace Hack {
	struct Player {
		int entityIndex = -1;
		string name;
		Vector originPos;
		bool hasHead = false;
		Vector headPos;
		float distToCrosshair;
	};
	typedef unordered_map<int, Player> PlayerList;
	struct GData {
		RECT screenSize;
		int localPlayerIndex = -1;
		shared_ptr<PlayerList> players;
		int closestToCrosshair = -1;
	};

	class GameData {
	private:
		bool shouldStop = false;
		HANDLE dataThreadHandle;

	public:
		static mutex dataMutex;
		static shared_ptr<GData> data;
		GameData();
		~GameData();

		void Stop() {
			shouldStop = true;
		}

		bool ShouldStop() {
			return shouldStop;
		}

		static Player GetPlayer(int entityIndex);

		static string GetName(int entityIndex);

		static float GetDistanceToCrosshair(IClientEntity* target, Vector targetPos);
	};
}