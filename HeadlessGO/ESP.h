#pragma once
#include "Drawing.h"
#include "SDK/SDK_Utilities.h"

#include "Menu.h"
using namespace std;

namespace Hack {
	class ESP
	{
	public:
		static void Render();

	private:
		static void DrawSnapline(IClientEntity* CurrentEnt, ImU32 color);
		static void DrawHead(IClientEntity* CurrentEnt, ImU32 color);
		static void DrawSegment(IClientEntity* CurrentEnt, string bone_1, string bone_2, ImU32 color);
		static void DrawSkeleton(IClientEntity* CurrentEnt, ImU32 boneColor, ImU32 headColor);
		static void DrawCrosshair(int width, int length, int offset, ImU32 color, float rounding = 0.f, bool outline = false, ImU32 outlineColor = 0, float outlineThickness = 1.f);
		static void DrawOrigin(IClientEntity* CurrentEnt, ImU32 color);
		static void DrawName(IClientEntity* CurrentEnt);
		static void DrawAimbotFOV(int FOV);
	};
}