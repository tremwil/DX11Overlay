// dllmain.cpp : Définit le point d'entrée pour l'application DLL.
#include "stdafx.h"
#include "DX11Renderer.h"
#include "PolyHook.h"
#include "Vector.h"
#include "Rendering\DXTKRenderer.h"
#include "Tools.h"

HMODULE DllInst = 0;
BOOL init = 0;
BOOL display = TRUE;
int timer = 0;

Vector2f textPos = Vector2f(100, 100);
Color textColor = Color(1, 0, 0);

DXTKRenderer* Renderer;
DX11Renderer* DXRend;

PLH::VTableSwap* PLHSwapChainhk;
typedef HRESULT(__stdcall* tPresent)(IDXGISwapChain* thisptr, UINT SyncInterval, UINT Flags);
tPresent oPresent;

typedef HRESULT(__stdcall* tResizeBuffers)(IDXGISwapChain* pThis, UINT BufferCount, UINT Width,
	UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
tResizeBuffers oResizeBuffers;

void graphicsSetup();
DWORD WINAPI InitThread(LPVOID lParam);


HRESULT __stdcall hkPresent(IDXGISwapChain* thisptr, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (timer < 200)
		{
			timer++;
			return oPresent(thisptr, SyncInterval, Flags);
		}
		graphicsSetup();
	}

	if (display)
	{
		Renderer->BeginText();
		Renderer->RenderText(textPos, textColor, "Here is some text...");
		Renderer->EndText();
	}

	return oPresent(thisptr, SyncInterval, Flags);
}


HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pThis, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	init = false;
	timer = 0;

	return oResizeBuffers(pThis, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}


void Uninject()
{
	PLHSwapChainhk->UnHook();
	TerminateThread(InitThread, 0);
	Sleep(200);
	FreeConsole();
	FreeLibraryAndExitThread(DllInst, 0);
}

void graphicsSetup()
{
	XTrace("Initializing Renderer\n");

	if (!PLH::IsValidPtr(DXRend))
		return;

	Renderer = new DXTKRenderer();
	Renderer->Init(DXRend->m_pScreen->m_pSwapChain, DXRend->m_pScreen->m_Width, DXRend->m_pScreen->m_Height);

	XTrace("Done\n");

	init = 1;
}


DWORD WINAPI InitThread(LPVOID lParam)
{
	CreateConsole();
	XTrace("Injected\n");
	DXRend = DX11Renderer::GetInstance();
	if (!PLH::IsValidPtr(DXRend))
		return 0;
	XTrace("Hooking\n");
	PLHSwapChainhk = new PLH::VTableSwap();
	PLHSwapChainhk->SetupHook((BYTE*)DXRend->m_pScreen->m_pSwapChain, 8, (BYTE*)&hkPresent);
	if (!PLHSwapChainhk->Hook())
	{
		XTrace("Hook failed");
		return 0;
	}
	oPresent = PLHSwapChainhk->GetOriginal<tPresent>();
	oResizeBuffers = PLHSwapChainhk->HookAdditional<tResizeBuffers>(13, (BYTE*)&hkResizeBuffers);
	XTrace("Done\n");

	while (1)
	{
		if (GetAsyncKeyState(VK_F10) & 0x8000)
		{
			Sleep(500);
			Uninject();
		}
		if (GetAsyncKeyState(VK_F11) & 0x8000)
		{
			Sleep(500);
			display = !display;
			if (display) XTrace("Display: ON\n");
			else XTrace("Display: OFF\n");
		}
		Sleep(20);
	}
	return 1;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD Reason, LPVOID lpReserve)
{
	if (Reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		DllInst = hModule;
		HANDLE hThread = CreateThread(NULL, NULL, InitThread, NULL, NULL, NULL);
		CloseHandle(hThread);
	}
	return TRUE;
}

