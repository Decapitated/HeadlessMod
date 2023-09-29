#include "Drawing.h"
using Hack::Drawing;
using Hack::Menu;
using Hack::ESP;
#pragma region Function Declarations

	HRESULT _stdcall Hooked_EndScene(IDirect3DDevice9* pDevice);
	LRESULT APIENTRY hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);

#pragma endregion

#pragma region Static Variables

	HWND    Drawing::processWindow{};
	WNDPROC Drawing::originalWndProc{};
	ImFont* Drawing::defaultFont{};
	HHOOK   Drawing::hMouseHook{};
	ImDrawList* Drawing::drawList{};

#pragma endregion

// Hooked function.
typedef HRESULT(_stdcall* f_EndScene)(IDirect3DDevice9* pDevice);
f_EndScene oEndScene = NULL;

Drawing::Drawing(LPCWSTR windowName)
{
	// Get process window.
	std::cout << "Getting process window." << std::endl;
	if ((processWindow = (HWND)FindWindow(0, windowName)) == NULL)
		throw "Failed to find process window.";

	// Get D3D9Device.
	std::cout << "Getting D3D9Device." << std::endl;
	if (!GetD3D9Device(d3d9Device, sizeof(d3d9Device), processWindow))
		throw "Failed to get D3D9Device";

	// Create a hook for EndScene, in disabled state.
	std::cout << "Creating drawing hook." << std::endl;
	#pragma warning(disable : 26812)
	if (MH_CreateHook((LPVOID)d3d9Device[42], &Hooked_EndScene, reinterpret_cast<LPVOID*>(&oEndScene)) != MH_OK)
		throw "Failed to create hook";

	// Enable the hook for EndScene.
	std::cout << "Enabling drawing hook." << std::endl;
	if (MH_EnableHook((LPVOID)d3d9Device[42]) != MH_OK)
		throw "Failed to enable hook";

	// Input hooks.
	// Allows clicking on ImGui menu when menu is open.
	originalWndProc = (WNDPROC)SetWindowLongPtr(processWindow, GWLP_WNDPROC, (__int3264)(LONG_PTR)hWndProc);
	// Allows clicks to be recorded for ImGui to use.
	hMouseHook = SetWindowsHookEx(WH_MOUSE, mouseProc, 0, GetCurrentThreadId()); // Our MouseInput Hook
}

Drawing::~Drawing()
{
	// Reset input hooks.
	SetWindowLongPtr(processWindow, GWLP_WNDPROC, (__int3264)(LONG_PTR)originalWndProc);
	UnhookWindowsHookEx(hMouseHook);

	// Disable the hook for EndScene.
	MH_DisableHook((LPVOID)d3d9Device[42]);

	// Cleanup ImGui.
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Drawing::InitImGui(IDirect3DDevice9* device)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	defaultFont = io.Fonts->AddFontDefault();

	ImGui_ImplWin32_Init(Drawing::processWindow);
	ImGui_ImplDX9_Init(device);
}

bool Drawing::GetD3D9Device(void** vTable, size_t size, HWND window)
{
	if (!vTable) return false;

	IDirect3D9* pObject = Direct3DCreate9(D3D_SDK_VERSION);
	if (!pObject) return false;

	IDirect3DDevice9* pDummyDevice = nullptr;

	D3DPRESENT_PARAMETERS pParams{};
	pParams.Windowed = false;
	pParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pParams.hDeviceWindow = window;

	HRESULT dummyCreated = pObject->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		pParams.hDeviceWindow,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&pParams,
		&pDummyDevice);

	if (dummyCreated != S_OK)
	{
		// may fail in windowed fullscreen mode, trying again with windowed mode
		pParams.Windowed = !pParams.Windowed;

		dummyCreated = pObject->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			pParams.hDeviceWindow,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&pParams,
			&pDummyDevice);

		if (dummyCreated != S_OK)
		{
			pObject->Release();
			return false;
		}
	}

	memcpy(vTable, *reinterpret_cast<void***>(pDummyDevice), size);

	pDummyDevice->Release();
	pObject->Release();
	return true;
}

#pragma region Hooks

	// Drawing goes in here.
	HRESULT _stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
	{
		#pragma region Called Once; ImGui init;

			// This section is only called once to initialize ImGui.
			static std::once_flag init;
			std::call_once(init, [&](IDirect3DDevice9* p_Device)
			{
				std::cout << "Initializing ImGui." << std::endl;
				Drawing::InitImGui(p_Device);
			}, pDevice);

		#pragma endregion

		ImGui::GetIO().WantCaptureMouse = Menu::bMenu; // Test if really needed.
		ImGui::GetIO().MouseDrawCursor = Menu::bMenu;  // Test if really needed.

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();

		/*
		 * Set the size of our frame rect. If too small, it will cause clipping.
		 * MUST BE SET AFTER imGui_ImplWin32_NewFrame().
		 * Not usually a problem unless using AllocConsole(),
		 * which makes GetClientRect() return console window size
		 * in the Win32_NewFrame function.
		 */
		int w, h;
		SourceInterfaces::pEngine->GetScreenSize(w, h);
		ImGui::GetIO().DisplaySize = ImVec2((float)w, (float)h);

		ImGui::NewFrame();

		Drawing::drawList = ImGui::GetBackgroundDrawList();

		#pragma region Draw Here

			if (SourceInterfaces::pEngine->IsInGame() &&
				SourceInterfaces::pEngine->IsConnected() &&
				!SourceInterfaces::pEngine->IsDrawingLoadingImage())
				ESP::Render();

			// Draw menu stuff.
			if (Menu::bMenu) Menu::Render();

		#pragma endregion

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		return oEndScene(pDevice); // Call original.
	}

	LRESULT APIENTRY hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (Menu::bMenu)
		{
			ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
			return true;
		}

		return CallWindowProc(Drawing::originalWndProc, hWnd, uMsg, wParam, lParam);
	}

	LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (Menu::bMenu) // bool
		{
			ImGuiIO& io = ImGui::GetIO();
			MOUSEHOOKSTRUCT* pMouseStruct = (MOUSEHOOKSTRUCT*)lParam;
			if (pMouseStruct != NULL) {
				switch (wParam)
				{
				case WM_LBUTTONDOWN:
					io.MouseDown[0] = true;
					break;
				case WM_LBUTTONUP:
					io.MouseDown[0] = false;
					io.MouseReleased[0] = true;
					break;
				case WM_RBUTTONDOWN:
					io.MouseDown[1] = true;
					break;
				case WM_RBUTTONUP:
					io.MouseDown[1] = false;
					io.MouseReleased[1] = true;
					break;
				}
			}
		}
		return CallNextHookEx(Drawing::hMouseHook, nCode, wParam, lParam);
	}

#pragma endregion

#pragma region Drawing

	void Drawing::Line(float x1, float y1, float x2, float y2, ImU32 color, float thickness)
	{
		//auto draw = ImGui::GetBackgroundDrawList();
		auto draw = Drawing::drawList;
		draw->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);
	}

	void Drawing::Circle(float x, float y, float radius, ImU32 color, int segments, float thickness)
	{
		//auto draw = ImGui::GetBackgroundDrawList();
		auto draw = Drawing::drawList;
		draw->AddCircle(ImVec2(x, y), radius, color, segments, thickness);
	}

	void Drawing::Box(float x, float y, float w, float h, ImU32 color, float thickness, float rounding)
	{
		//auto draw = ImGui::GetBackgroundDrawList();
		auto draw = Drawing::drawList;
		draw->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding, 0, thickness);
	}

	void Drawing::BoxFilled(float x, float y, float w, float h, ImU32 color, float rounding)
	{
		//auto draw = ImGui::GetBackgroundDrawList();
		auto draw = Drawing::drawList;
		draw->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding);
	}

	void Drawing::OutlinedFilledBox(float x, float y, float w, float h, ImU32 color, ImU32 outlineColor, float outlineThickness, float rounding)
	{
		//auto draw = ImGui::GetBackgroundDrawList();
		auto draw = Drawing::drawList;
		draw->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, rounding);
		draw->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), outlineColor, rounding, 0, outlineThickness);
	}

	void Drawing::Text(char* text, float x, float y, ImU32 color)
	{
		int length = strlen(text);
		//auto draw = ImGui::GetBackgroundDrawList();
		auto draw = Drawing::drawList;
		draw->AddText(Drawing::defaultFont, 0, ImVec2(x, y), color, text, NULL, NULL, NULL);
	}

#pragma endregion