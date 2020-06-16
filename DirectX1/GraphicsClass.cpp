#include"Stdafx.h"
#include "D3DClass.h"
#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenW, int screenH, HWND hWnd, bool isFullScreen)
{
	mDirect3D = new D3DClass;
	if (!mDirect3D)
		return false;

	if (!mDirect3D->Initialize(screenW, screenH, true, hWnd, isFullScreen, 1000.0f, 0.1f))
	{
		MessageBox(hWnd, L"Direct3D Failed", L"¿À·ù", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Release()
{
	if (mDirect3D)
	{
		mDirect3D->Release();
		delete mDirect3D;
		mDirect3D = 0;
	}
}

bool GraphicsClass::Frame()
{
	bool result;
	if (!Render()) 
		return false;

	return Render();
}
bool GraphicsClass::Render()
{
	mDirect3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	mDirect3D->EndScene();
	return true;
}