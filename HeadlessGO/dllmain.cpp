// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
// Windows Header Files
#include <windows.h>
#include <iostream> // For console output.
#include <unordered_map>

#include "GameData.h"
#include "Drawing.h"
#include "Aimbot.h"

#pragma region Function Declarations

DWORD WINAPI MainThread(HMODULE hModule);

#pragma endregion

BOOL APIENTRY DllMain( HMODULE _hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, _hModule, 0, nullptr));
    }
    return TRUE;
}

DWORD WINAPI MainThread(HMODULE hModule)
{
    #pragma region Console Setup

        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);

    #pragma endregion

    try
    {
        // Initialize MinHook.
        #pragma warning(disable : 26812)
        if (MH_Initialize() != MH_OK)
            throw "Failed to initialize MinHook";

        // Initialize interfaces with GetInterface().
        if (CreateInterface().Intitialize())
        {
            auto gamedata = make_unique<Hack::GameData>();              // Initialize GameData loop.
            // Initialize Menu and ESP.
            auto draw = make_unique<Hack::Drawing>(
            #if _WIN64
                L"Garry's Mod (x64)"
            #else
                L"Garry's Mod"
            #endif
            );
            
            auto aimbot   = make_unique<Hack::Aimbot>();                // Initialize Aimbot.

            while (true)
            {
                // End loop.
                if (GetAsyncKeyState(VK_INSERT))
                    break;

                // Open menu.
                if (GetAsyncKeyState(VK_HOME) & 0x11)
                    Hack::Menu::bMenu = !Hack::Menu::bMenu;

                Sleep(100); // Sleep since we don't do much in this thread.
            }
        }
    }
    catch (char* e)
    {
        std::cout << e << std::endl;
        char ignore = getchar();
    }

    #pragma region Cleanup

        // Uninitialize MinHook.
        MH_Uninitialize();

        // Cleanup console.
        if (f) fclose(f);
        FreeConsole();

        // Cleanup dll for uninjection.
        FreeLibraryAndExitThread(hModule, 0);
        return 0;

    #pragma endregion
}