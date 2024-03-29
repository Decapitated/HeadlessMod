#pragma once
#include "imgui.h"
#include "SDK/SDK_Utilities.h"

namespace Hack {
	class Menu
	{
	public:
		static void Render();

		static const float MIN_SIZE_FLOAT;
		static const float MAX_SIZE_FLOAT;

	#pragma region Feature Toggles

		static bool bMenu;
		static bool bSnapline;
		static bool bSkeleton;
		static bool bOrigin;
		static bool bName;
		static bool bShowTeam;
		static bool bShowNPC;
		static bool bViewPlayers;
		static bool bViewVisiblePlayers;

		static bool bCrosshair;
		static bool bCrosshairOutline;

		static bool bAimbot;

	#pragma endregion

	#pragma region Feature Colors

		static ImVec4 cSkeletonHead;
		static ImVec4 cSkeletonBody;
		static ImVec4 cSnapline;
		static ImVec4 cOrigin;

		static ImVec4 cCrosshair;
		static ImVec4 cCrosshairOutline;

	#pragma endregion

	#pragma region Feature Values

		static float vCrosshairOffset;
		static float vCrosshairWidth;
		static float vCrosshairHeight;
		static float vCrosshairRounding;
		static float vCrosshairOutlineThickness;

		static int vAimbotSmoothing;
		static int vAimbotFOV;

	#pragma endregion

		static ImU32 ToColor(ImVec4* color)
		{
			return IM_COL32(color->x * 255, color->y * 255, color->z * 255, color->w * 255);
		}

	private:
		static const ImGuiColorEditFlags colorPickerFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview;

		static void MainWindow();
		static void PlayersWindow();
		static void VisiblePlayersWindow();
	};
}