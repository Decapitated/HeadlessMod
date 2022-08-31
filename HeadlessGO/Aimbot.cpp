#include "Aimbot.h"

#pragma region Function Declarations

    bool __fastcall CreateMoveFn(void* ecx, void* edx, float SampleTime, CUserCmd* cmd);

#pragma endregion

// Hooked function.
typedef bool(__thiscall* createMove_t)(void*, float, CUserCmd*);
createMove_t oCreateMove;

Aimbot::Aimbot()
{
    ClientModeShared* clientModeShared = **reinterpret_cast<ClientModeShared***>((*reinterpret_cast<uintptr_t**>(SourceInterfaces::pClient))[10] + 5);
    memcpy(vt_clientShared, *reinterpret_cast<void***>(clientModeShared), sizeof(vt_clientShared));

    // Create a hook for CreateMove.
    std::cout << "Creating aimbot hook." << std::endl;
    #pragma warning(disable : 26812)
    if (MH_CreateHook((LPVOID)vt_clientShared[21], &CreateMoveFn, reinterpret_cast<LPVOID*>(&oCreateMove)) != MH_OK)
        throw "Failed to create aimbot hook";

    // Enable the hook for CreateMove.
    std::cout << "Enabling aimbot hook." << std::endl;
    if (MH_EnableHook((LPVOID)vt_clientShared[21]) != MH_OK)  throw "Failed to enable aimbot hook";
}

Aimbot::~Aimbot()
{
    // Disable the hook for CreateMove.
    MH_DisableHook((LPVOID)vt_clientShared[21]);
}

#pragma region Hook

    bool __fastcall CreateMoveFn(void* ecx, void* edx, float SampleTime, CUserCmd* cmd)
    {
        if (!cmd->m_cmd_nr)
            return oCreateMove(ecx, SampleTime, cmd);

        if (Menu::bAimbot &&
            SourceInterfaces::pEngine->IsInGame() &&
            SourceInterfaces::pEngine->IsConnected())
        {
            // Mouse side buttons.
            if (GetAsyncKeyState(VK_XBUTTON1) || GetAsyncKeyState(VK_XBUTTON2))
            {
                IClientEntity* localPlayer = SDK_Utilities::GetLocalPlayer();
                int localHead = SDK_Utilities::GrabBone(localPlayer, SDK_Utilities::getBoneArray()[13]);
                Vector localPos = (localHead == -1 ? localPlayer->GetOrigin() : SDK_Utilities::GetBonePos(localPlayer, localHead));

                Vector targetPos;
                IClientEntity* target = SDK_Utilities::GrabClosestEntToCrosshair(targetPos);
                if (target)
                {
                    QAngle aimAngle = SDK_Utilities::Math::GetAngle(localPos, targetPos);
                    cmd->m_viewangles.x = aimAngle.x;
                    cmd->m_viewangles.y = aimAngle.y;
                }
            }
        }
        return true;
    }

#pragma endregion