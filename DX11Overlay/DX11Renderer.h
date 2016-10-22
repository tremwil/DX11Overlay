#pragma once

#define OFFSET_DX11RENDERER 0x142563300;
#include <d3d11.h>

class Screen
{
public:
	char _0x0000[584];
	__int32 m_Width; //0x0248 
	__int32 m_Height; //0x024C 
	__int32 m_WindowWidth; //0x0250 
	__int32 m_WindowHeight; //0x0254 
	char _0x0258[32];
	IDXGISwapChain* m_pSwapChain; //0x0278 
};//Size=0x0580

class DX11Renderer
{
public:
	char _0x0000[2016];
	Screen* m_pScreen; //0x07E0 
	char _0x07E8[112];
	ID3D11Device* m_pDevice; //0x0858 
	char _0x0860[32];
	static DX11Renderer* GetInstance()
	{
		return *(DX11Renderer**)OFFSET_DX11RENDERER;
	}
};//Size=0x0880