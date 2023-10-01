#include "Aimbot.h"
using Hack::Aimbot;
using Hack::Menu;
using Hack::GameData;

#pragma region Function Declarations

    bool __fastcall CreateMoveFn(void* ecx, void* edx, float SampleTime, CUserCmd* cmd);
    float ease(float x);
    float easeInOutBack(float x);
    float easeOutBounce(float x);
    float easeOutQuart(float x);
    float easeInOutExpo(float x);
    float easeInOutSine(float x);

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
                const std::lock_guard<std::mutex> lock(GameData::dataMutex);
                shared_ptr<Hack::GData> data = GameData::data;

                Hack::Player localPlayer = data->players->at(data->localPlayerIndex);

                if (data->closestToCrosshair != -1) {
                    Hack::Player targetPlayer = data->players->at(data->closestToCrosshair);
                    QAngle aimAngle = SDK_Utilities::Math::GetAngle(localPlayer.headPos, targetPlayer.headPos);
                    QAngle diffs = aimAngle - QAngle{ cmd->m_viewangles.x, cmd->m_viewangles.y, cmd->m_viewangles.z };
                    float diff_x = diffs.x, diff_y = diffs.y;
                    if (diff_x < 0) diff_x *= -1;
                    if (diff_y < 0) diff_y *= -1;
                    float dist = diff_x + diff_y;

                    if (dist <= Menu::vAimbotFOV) {
                        float norm_dist = SDK_Utilities::Math::NormalizeValue(0, Menu::vAimbotFOV, dist);
                        // Ease
                        //float smooth = ((Menu::vAimbotSmoothing - 1) * ease(norm_dist)) + 1;
                        float smooth = Menu::vAimbotSmoothing;

                        cmd->m_viewangles.x = cmd->m_viewangles.x + (diffs.x / max(smooth, 1.0f));
                        cmd->m_viewangles.y = cmd->m_viewangles.y + (diffs.y / max(smooth, 1.0f));
                    }
                }
            }
        }
        return true;
    }

    float ease(float x) {
        return easeInOutSine(x);
    }

    float easeInOutBack(float x) {
        float c1 = 1.70158f;
        float c2 = c1 * 1.525f;

        if (x < 0.5) {
            return (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2;
        }
        else {
            return (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
        }
    }
    
    float easeOutBounce(float x) {
        float n1 = 7.5625f;
        float d1 = 2.75f;

        if (x < 1.f / d1) {
            return n1 * x * x;
        }
        else if (x < 2.f / d1) {
            return n1 * (x -= 1.5f / d1) * x + 0.75f;
        }
        else if (x < 2.5f / d1) {
            return n1 * (x -= 2.25f / d1) * x + 0.9375f;
        }
        else {
            return n1 * (x -= 2.625f / d1) * x + 0.984375f;
        }
    }

    float easeOutQuart(float x) {
        return 1 - pow(1 - x, 4);
    }

    float easeInOutExpo(float x) {
        if (x == 0) {
            return 0;
        }
        else if (x == 1) {
            return 1;
        }
        else if (x < 0.5f) {
            return pow(2, 20 * x - 10) / 2;
        }
        else {
            return (2 - pow(2, -20 * x + 10)) / 2;
        }
    }

    float easeInOutSine(float x) {
        return -(cos(3.1415926535897932384626433832795 * x) - 1) / 2;
    }
#pragma endregion