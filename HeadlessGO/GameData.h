#pragma once
#include <iostream> // For console output.
#include <unordered_map>
#include "SDK/SDK_Utilities.h"
#include "Menu.h"

namespace Hack {
	struct Player {
		int EntityIndex;
		char* Name;
	};

	struct GData {
		int localPlayerIndex = -1;
		std::unordered_map<int, Player> players{};
	};

	class GameData {
	private:
		bool shouldStop = false;
		HANDLE dataThreadHandle;
		GData data;

	public:

		GameData();
		~GameData();

		void Stop() {
			shouldStop = true;
		}

		bool ShouldStop() {
			return shouldStop;
		}
		
		void SetData(GData* newData) {
			data = *newData;
		}

		GData GetData() {
			return data;
		}

		static char* GetName(int entityIndex);

	};
}