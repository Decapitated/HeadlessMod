#include "ESP.h"
using Hack::ESP;
using Hack::Menu;
using Hack::Drawing;

void ESP::Render()
{
	int maxEntities = SourceInterfaces::pEntityList->GetHighestEntityIndex();
	IClientEntity* localEntity = SDK_Utilities::GetLocalPlayer();
	for (int i = 0; i <= maxEntities; i++)
	{
		IClientEntity* currentEntity = SourceInterfaces::pEntityList->GetClientEntity(i);

		if (i == SourceInterfaces::pEngine->GetLocalPlayer() ||
			!SDK_Utilities::IsValid(currentEntity, nullptr, Menu::bShowNPC))
		{
			continue;
		}

		if (!Menu::bShowTeam && currentEntity->GetTeam() == localEntity->GetTeam())
			continue;

		if (Menu::bSnapline)
			DrawSnapline(currentEntity, Menu::ToColor(&Menu::cSnapline));

		if (Menu::bSkeleton)
			DrawSkeleton(currentEntity, Menu::ToColor(&Menu::cSkeletonBody), Menu::ToColor(&Menu::cSkeletonHead));

		if (Menu::bOrigin)
			DrawOrigin(currentEntity, Menu::ToColor(&Menu::cOrigin));

		if (Menu::bName)
			DrawName(currentEntity);
	}

	if (Menu::bAimbot)
		DrawAimbotFOV(Menu::vAimbotFOV);

	// Draw croshair last so it's always on top.
	if (Menu::bCrosshair)
		DrawCrosshair(
			(int)Menu::vCrosshairWidth,                   // Width
			(int)Menu::vCrosshairHeight,                  // Height
			(int)Menu::vCrosshairOffset,                  // Offset
			Menu::ToColor(&Menu::cCrosshair),        // Main Color
			Menu::vCrosshairRounding,                // Rounding
			Menu::bCrosshairOutline,                 // Outline?
			Menu::ToColor(&Menu::cCrosshairOutline), // Outline Color
			Menu::vCrosshairOutlineThickness);       // Outline Thickness
}

#pragma region Drawing

	void ESP::DrawSnapline(IClientEntity* CurrentEnt, ImU32 color)
	{
		Vector WorldToScreenEnt, CurrentOrg;

		CurrentOrg = CurrentEnt->GetOrigin();

		if (SDKMath::WorldToScreen(CurrentOrg, WorldToScreenEnt))
		{
			int w, h;
			SourceInterfaces::pEngine->GetScreenSize(w, h);
			Drawing::Line(WorldToScreenEnt.x, WorldToScreenEnt.y, w / 2.f, (float)h, color, 1.f);
		}
	}

	void ESP::DrawHead(IClientEntity* CurrentEnt, ImU32 color)
	{
		string head = SDK_Utilities::getBoneArray()[13];
		int boneIndex = SDK_Utilities::GrabBone(CurrentEnt, head.c_str());
		if (boneIndex != -1)
		{
			Vector bonePos = SDK_Utilities::GetBonePos(CurrentEnt, boneIndex);
			bonePos.z = bonePos.z + 2.f;

			float distance = SDKMath::GetDistanceBetween(SDK_Utilities::GetLocalPlayer()->GetOrigin(), bonePos);
			float offset = 3000.0f / distance;

			Vector w_bonePos;
			if (SDKMath::WorldToScreen(bonePos, w_bonePos))
			{
				Drawing::Box(w_bonePos.x - offset, w_bonePos.y - offset, offset * 2, offset * 2, color, 2.f);
			}
		}
	}

	void ESP::DrawSegment(IClientEntity* CurrentEnt, string bone_1, string bone_2, ImU32 color)
	{
		int bone_a = SDK_Utilities::GrabBone(CurrentEnt, bone_1.c_str());
		int bone_b = SDK_Utilities::GrabBone(CurrentEnt, bone_2.c_str());
		if (bone_a != -1 && bone_b != -1)
		{
			Vector bonePos_a = SDK_Utilities::GetBonePos(CurrentEnt, bone_a);
			Vector bonePos_b = SDK_Utilities::GetBonePos(CurrentEnt, bone_b);
			Vector w_bonePos_a, w_bonePos_b;
			if (SDKMath::WorldToScreen(bonePos_a, w_bonePos_a) && SDKMath::WorldToScreen(bonePos_b, w_bonePos_b))
			{
				Drawing::Line(w_bonePos_a.x, w_bonePos_a.y, w_bonePos_b.x, w_bonePos_b.y, color, 2.f);
			}
		}
	}

	void ESP::DrawSkeleton(IClientEntity* CurrentEnt, ImU32 boneColor, ImU32 headColor)
	{
		/*
		//Right Leg
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[21], SDK_Utilities::getBoneArray()[20], boneColor); // R Toe -> R Foot
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[20], SDK_Utilities::getBoneArray()[19], boneColor); // R Foot -> R Calf
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[19], SDK_Utilities::getBoneArray()[18], boneColor); // R Calf -> R Thigh
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[18], SDK_Utilities::getBoneArray()[0], boneColor); // R Thigh -> Pelvis
		//Left Leg
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[17], SDK_Utilities::getBoneArray()[16], boneColor); // L Toe -> L Foot
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[16], SDK_Utilities::getBoneArray()[15], boneColor); // L Foot -> L Calf
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[15], SDK_Utilities::getBoneArray()[14], boneColor); // L Calf -> L Thigh
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[14], SDK_Utilities::getBoneArray()[0], boneColor); // L Thigh -> Pelvis
		//Spine
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[0], SDK_Utilities::getBoneArray()[3], boneColor); // Pelvis -> Spine 2
		//Right Arm
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[11], SDK_Utilities::getBoneArray()[10], boneColor); // R Hand -> R Forearm
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[10], SDK_Utilities::getBoneArray()[9], boneColor); // R Forearm -> R Upperarm
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[9], SDK_Utilities::getBoneArray()[3], boneColor); // R Upperarm -> Spine 2
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[9], SDK_Utilities::getBoneArray()[13], boneColor); // R Upperarm -> Head
		//Left Arm
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[7], SDK_Utilities::getBoneArray()[6], boneColor); // L Hand -> L Forearm
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[6], SDK_Utilities::getBoneArray()[5], boneColor); // L Forearm -> L Upperarm
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[5], SDK_Utilities::getBoneArray()[3], boneColor); // L Upperarm -> Spine 2
		DrawSegment(CurrentEnt, SDK_Utilities::getBoneArray()[5], SDK_Utilities::getBoneArray()[13], boneColor); // L Upperarm -> Head*/
		//Head
		DrawHead(CurrentEnt, headColor);
	}

	void ESP::DrawCrosshair(int width, int length, int offset, ImU32 color, float rounding, bool outline, ImU32 outlineColor, float outlineThickness)
	{
		int w, h;
		SourceInterfaces::pEngine->GetScreenSize(w, h);

		float x = w / 2.f, y = h / 2.f;
		if (outline)
		{
			Drawing::OutlinedFilledBox(
				x - width / 2.f, y - offset - length, // X, Y
				(float)width, (float)length, color, outlineColor, outlineThickness, rounding);
			Drawing::OutlinedFilledBox(
				x + offset, y - width / 2.f,
				(float)length, (float)width, color, outlineColor, outlineThickness, rounding);
			Drawing::OutlinedFilledBox(
				x - width / 2.f, y + offset,
				(float)width, (float)length, color, outlineColor, outlineThickness, rounding);
			Drawing::OutlinedFilledBox(x - offset - length, y - width / 2.f,
				(float)length, (float)width, color, outlineColor, outlineThickness, rounding);
		}
		else
		{
			Drawing::BoxFilled(
				x - width / 2.f, y - offset - length, // X, Y
				(float)width, (float)length, color, rounding);
			Drawing::BoxFilled(
				x + offset, y - width / 2.f,
				(float)length, (float)width, color, rounding);
			Drawing::BoxFilled(
				x - width / 2.f, y + offset,
				(float)width, (float)length, color, rounding);
			Drawing::BoxFilled(
				x - offset - length, y - width / 2.f,
				(float)length, (float)width, color, rounding);
		}
	}

	void ESP::DrawOrigin(IClientEntity* CurrentEnt, ImU32 color)
	{
		Vector WorldToScreenEnt, CurrentOrg;

		CurrentOrg = CurrentEnt->GetOrigin();

		if (SDKMath::WorldToScreen(CurrentOrg, WorldToScreenEnt))
		{
			Drawing::Circle(WorldToScreenEnt.x, WorldToScreenEnt.y, 5.f, color, 5, 2.f);
		}
	}

	void ESP::DrawName(IClientEntity* CurrentEnt)
	{
		Vector out;
		player_info_t pinfo;
		Vector WorldToScreenEnt, CurrentOrg;

		CurrentOrg = CurrentEnt->GetOrigin();

		if (SDKMath::WorldToScreen(CurrentOrg, WorldToScreenEnt))
		{
			SourceInterfaces::pEngine->GetPlayerInfo(CurrentEnt->entindex(), &pinfo);
			ImU32 color;
			int health = CurrentEnt->GetHealth();
			if (health >= 100)
			{
				color = IM_COL32(0, 255, 0, 255); // Green
			}
			else if (health > 15)
			{
				color = IM_COL32(150, 150, 0, 255); // Yellow?
			}
			else
			{
				color = IM_COL32(255, 0, 0, 255); // Red
			}
			int length = strlen(pinfo.Name);
			float width = (length * 13) / 2.f;
			Drawing::Text(pinfo.Name, WorldToScreenEnt.x - (width / 2.f), WorldToScreenEnt.y, color);
		}
	}

	void ESP::DrawAimbotFOV(int FOV) {
		int w, h;
		SourceInterfaces::pEngine->GetScreenSize(w, h);

		float x = w / 2.f, y = h / 2.f;

		Drawing::Circle(x, y, (float)FOV*15, Menu::ToColor(&Menu::cCrosshair));
	}
#pragma endregion