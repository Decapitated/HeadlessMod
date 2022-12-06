#include "Menu.h"

const float Menu::MAX_SIZE_FLOAT{ 1000.f };
const float Menu::MIN_SIZE_FLOAT{ 1.f };

#pragma region Feature Toggle Defaults

	//Start with ____ on/off.
	bool Menu::bMenu{ false };
	bool Menu::bSnapline{ false };
	bool Menu::bSkeleton{ false };
	bool Menu::bOrigin{ false };
	bool Menu::bName{ false };
	bool Menu::bShowTeam{ true };
	bool Menu::bShowNPC{ true };
	bool Menu::bViewPlayers{ false };
	bool Menu::bViewVisiblePlayers{ false };

	bool Menu::bCrosshair{ true };
	bool Menu::bCrosshairOutline{ true };

	bool Menu::bAimbot{ false };

#pragma endregion

#pragma region Feature Color Defaults

	ImVec4 Menu::cSkeletonBody{ 0, 0, 1.f, 1.f }; // Default Blue
	ImVec4 Menu::cSkeletonHead{ 0, 1.f, 0, 1.f }; // Default Green
	ImVec4 Menu::cSnapline{ 0, 1.f, 0, 1.f }; // Default Green
	ImVec4 Menu::cOrigin{ 0, 1.f, 0, 1.f }; // Default Green

	ImVec4 Menu::cCrosshair{ 1.f, 0, 1.f, 1.f }; // Default Purple
	ImVec4 Menu::cCrosshairOutline{ 0, 1.f, 0, 1.f }; // Default Green

#pragma endregion

#pragma region Feature Value Defaults

	float Menu::vCrosshairOffset{ 5.f };
	float Menu::vCrosshairWidth{ 4.f };
	float Menu::vCrosshairHeight{ 10.f };
	float Menu::vCrosshairRounding{ 0.f };
	float Menu::vCrosshairOutlineThickness{ 1.f };

	int Menu::vAimbotSmoothing{ 10 };
	int Menu::vAimbotFOV{ 10 };

#pragma endregion

void Menu::Render()
{
	MainWindow();

	// Stuff only to be shown when player is in-game.
	if (SourceInterfaces::pEngine->IsInGame() &&
		SourceInterfaces::pEngine->IsConnected() &&
		!SourceInterfaces::pEngine->IsDrawingLoadingImage())
	{
		if (bViewPlayers)
			PlayersWindow();

		if (bViewVisiblePlayers)
			VisiblePlayersWindow();
	}
}

#pragma region Windows

	void Menu::MainWindow()
	{
		if (ImGui::Begin("HeadlessMOD"))
		{
			#pragma region Crosshair

				if (bCrosshair)
				{
					if (ImGui::TreeNode("Crosshair"))
					{
						ImGui::Checkbox("Enable/Disable", &bCrosshair);

						// Crosshair color picker.
						ImGui::Text("Main Color"); ImGui::SameLine();
						ImGui::ColorEdit4("Crosshair Main Color", (float*)&cCrosshair, colorPickerFlags);

						ImGui::SliderScalar("Width", ImGuiDataType_Float, &vCrosshairWidth, &MIN_SIZE_FLOAT, &MAX_SIZE_FLOAT);
						ImGui::SliderScalar("Height", ImGuiDataType_Float, &vCrosshairHeight, &MIN_SIZE_FLOAT, &MAX_SIZE_FLOAT);
						ImGui::SliderScalar("Offset", ImGuiDataType_Float, &vCrosshairOffset, &MIN_SIZE_FLOAT, &MAX_SIZE_FLOAT);
						ImGui::SliderScalar("Rounding", ImGuiDataType_Float, &vCrosshairRounding, &MIN_SIZE_FLOAT, &MAX_SIZE_FLOAT);

						if (bCrosshairOutline)
						{
							if (ImGui::TreeNode("Crosshair Outline"))
							{
								ImGui::Checkbox("Enable/Disable", &bCrosshairOutline);

								// Crosshair outline color picker.
								ImGui::Text("Outline Color"); ImGui::SameLine();
								ImGui::ColorEdit4("Crosshair Outline Color", (float*)&cCrosshairOutline, colorPickerFlags);

								ImGui::SliderScalar("Thickness", ImGuiDataType_Float, &vCrosshairOutlineThickness, &MIN_SIZE_FLOAT, &MAX_SIZE_FLOAT);

								// Tree end.
								ImGui::TreePop();
							}
						}
						else
						{
							ImGui::Checkbox("Crosshair Outline", &bCrosshairOutline);
						}

						// Tree end.
						ImGui::TreePop();
					}
				}
				else
				{
					ImGui::Checkbox("Crosshair", &bCrosshair);
				}

			#pragma endregion
			#pragma region Snapline

				if (bSnapline)
				{
					if (ImGui::TreeNode("Snapline"))
					{
						ImGui::Checkbox("Enable/Disable", &bSnapline);
						// Crosshair color picker.
						ImGui::Text("Color"); ImGui::SameLine();
						ImGui::ColorEdit4("Snapline Color", (float*)&cSnapline, colorPickerFlags);

						// Tree end.
						ImGui::TreePop();
					}
				}
				else
				{
					ImGui::Checkbox("Snapline", &bSnapline);
				}

			#pragma endregion
			#pragma region Skeleton

				if (bSkeleton)
				{
					if (ImGui::TreeNode("Skeleton"))
					{
						ImGui::Checkbox("Enable/Disable", &bSkeleton);
						// Head color picker.
						ImGui::Text("Head Color"); ImGui::SameLine();
						ImGui::ColorEdit4("Head Color", (float*)&cSkeletonHead, colorPickerFlags);
						// Body color picker.
						ImGui::Text("Body Color"); ImGui::SameLine();
						ImGui::ColorEdit4("Body Color", (float*)&cSkeletonBody, colorPickerFlags);

						// Tree end.
						ImGui::TreePop();
					}
				}
				else
				{
					ImGui::Checkbox("Skeleton", &bSkeleton);
				}

			#pragma endregion
			#pragma region Origin

				if (bOrigin)
				{
					if (ImGui::TreeNode("Origin"))
					{
						ImGui::Checkbox("Enable/Disable", &bOrigin);
						// Crosshair color picker.
						ImGui::Text("Color"); ImGui::SameLine();
						ImGui::ColorEdit4("Origin Color", (float*)&cOrigin, colorPickerFlags);

						// Tree end.
						ImGui::TreePop();
					}
				}
				else
				{
					ImGui::Checkbox("Origin", &bOrigin);
				}

			#pragma endregion
			ImGui::Checkbox("Name", &bName);
			ImGui::Checkbox("Show Team", &bShowTeam);
			ImGui::Checkbox("Show NPC", &bShowNPC);
			ImGui::Checkbox("View Players", &bViewPlayers);
			ImGui::Checkbox("View Visible Players", &bViewVisiblePlayers);
			#pragma region Aimbot

				if (bAimbot)
				{
					if (ImGui::TreeNode("Aimbot"))
					{
						ImGui::Checkbox("Enable/Disable", &bAimbot);
						// Aimbot Smoothing.
						ImGui::SliderInt("Smoothing", &vAimbotSmoothing, 1, 50);
						// Aimbot FOV.
						ImGui::SliderInt("FOV", &vAimbotFOV, 1, 50);

						// Tree end.
						ImGui::TreePop();
					}
				}
				else
				{
					ImGui::Checkbox("Aimbot", &bAimbot);
				}

			#pragma endregion
		} ImGui::End();
	}

	void Menu::PlayersWindow()
	{
		if (ImGui::Begin("Players"))
		{
			// Name; Health; Team; Class ID;
			if (ImGui::BeginTable("tPlayers", 4))
			{
				// Headers
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Health");
				ImGui::TableSetupColumn("Team");
				ImGui::TableSetupColumn("Class ID");
				ImGui::TableHeadersRow();

				int maxEntities = SourceInterfaces::pEntityList->GetHighestEntityIndex();
				for (int i = 0; i <= maxEntities; i++)
				{
					IClientEntity* currentEntity = SourceInterfaces::pEntityList->GetClientEntity(i);
					int classID;
					if (!SDK_Utilities::IsValid(currentEntity, &classID))
					{
						continue;
					}
					player_info_t pinfo;
					SourceInterfaces::pEngine->GetPlayerInfo(currentEntity->entindex(), &pinfo);

					ImGui::TableNextRow();
					// Name
					ImGui::TableNextColumn();
					ImGui::Text("%s", pinfo.Name);
					// Health
					ImGui::TableNextColumn();
					ImGui::Text("%d", currentEntity->GetHealth());
					// Team
					ImGui::TableNextColumn();
					ImGui::Text("%d", currentEntity->GetTeam());
					// Class ID
					ImGui::TableNextColumn();
					ImGui::Text("%d", classID);
				}
				ImGui::EndTable();
			}
		} ImGui::End();
	}

	void Menu::VisiblePlayersWindow()
	{
		if (ImGui::Begin("Visible Players"))
		{
			// Name; Health; Team; Class ID;
			if (ImGui::BeginTable("tVisiblePlayers", 4))
			{
				// Headers
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Health");
				ImGui::TableSetupColumn("Team");
				ImGui::TableSetupColumn("Class ID");
				ImGui::TableHeadersRow();

				int maxEntities = SourceInterfaces::pEntityList->GetHighestEntityIndex();
				for (int i = 0; i <= maxEntities; i++)
				{
					IClientEntity* currentEntity = SourceInterfaces::pEntityList->GetClientEntity(i);
					int classID;
					if (!SDK_Utilities::IsValid(currentEntity, &classID, true))
					{
						continue;
					}

					if (SDK_Utilities::GetVisible(currentEntity))
					{
						player_info_t pinfo;
						SourceInterfaces::pEngine->GetPlayerInfo(currentEntity->entindex(), &pinfo);

						ImGui::TableNextRow();
						// Name
						ImGui::TableNextColumn();
						ImGui::Text("%s", pinfo.Name);
						// Health
						ImGui::TableNextColumn();
						ImGui::Text("%d", currentEntity->GetHealth());
						// Team
						ImGui::TableNextColumn();
						ImGui::Text("%d", currentEntity->GetTeam());
						// Class ID
						ImGui::TableNextColumn();
						ImGui::Text("%d", classID);
					}
				}
				ImGui::EndTable();
			}
		} ImGui::End();
	}

#pragma endregion