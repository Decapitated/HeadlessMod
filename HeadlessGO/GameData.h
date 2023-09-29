#pragma once
#include <iostream> // For console output.
#include <unordered_map>
#include "SDK/SDK_Utilities.h"
#include "Menu.h"

namespace Hack {
	struct Player {
		int entityIndex = -1;
		string name;
	};

	struct GData {
		int localPlayerIndex = -1;
		std::unordered_map<int, Player> players{};
		int closestToCrosshair = -1;
		Vector targetPos;
	};

	class GameData {
	private:
		bool shouldStop = false;
		HANDLE dataThreadHandle;

	public:
		static GData* data;
		GameData();
		~GameData();

		void Stop() {
			shouldStop = true;
		}

		bool ShouldStop() {
			return shouldStop;
		}

		static char* GetName(int entityIndex);

	};
}