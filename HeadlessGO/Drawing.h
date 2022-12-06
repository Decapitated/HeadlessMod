#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "imgui.h"
#include "imgui/backends/imgui_impl_dx9.h"
#include "imgui/backends/imgui_impl_win32.h"

#include "Minhook/MinHook.h"
#if _WIN64
	#pragma comment(lib, "MinHook.x64.lib")
#else
	#pragma comment(lib, "MinHook.x86.lib")
#endif


#include <mutex>
#include <iostream>

#include "ESP.h"
#include "Menu.h"

class Drawing
{
public:
	Drawing(LPCWSTR windowName);
	~Drawing();

	static HWND processWindow;
	static WNDPROC originalWndProc;
	static ImFont* defaultFont;
	static HHOOK hMouseHook;

	static void InitImGui(IDirect3DDevice9* device);

	static void Line(float x1, float y1, float x2, float y2, ImU32 color, float thickness = 1.0f);
	static void Circle(float x, float y, float radius, ImU32 color, int segments = 0, float thickness = 1.f);
	static void Box(float x, float y, float w, float h, ImU32 color, float thickness = 1.f, float rounding = 0.f);
	static void BoxFilled(float x, float y, float w, float h, ImU32 color, float rounding = 0.f);
	static void OutlinedFilledBox(float x, float y, float w, float h, ImU32 color, ImU32 outlineColor, float outlineThickness = 1.f, float rounding = 0.f);
	static void Text(char* text, float x, float y, ImU32 color);

private:
	// DirectX vTable.
	void* d3d9Device[43];

	bool GetD3D9Device(void** vTable, size_t size, HWND window);
};

extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);