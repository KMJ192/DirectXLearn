#include"Stdafx.h"
#include "D3DClass.h"
#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	_SDepth = 1000.0f;
	_SNear = 0.1f;
	_D3DC = NULL;
	_Vsync_enabled = true;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenW, int screenH, HWND hWnd, bool isFullScreen)
{
	bool result = false;

	_D3DC = new D3DClass;
	if (!_D3DC)
		return false;
	result = _D3DC->Initialize(screenW, screenH, _Vsync_enabled, hWnd, isFullScreen, _SDepth, _SNear);
	if (!result)
	{
		MessageBox(hWnd, _T("Could not initialize Direct3D"), _T("Error"), MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Release()
{
	//D3D객체 해제
	if (_D3DC)
	{
		_D3DC->Release();
		delete _D3DC;
		_D3DC = NULL;
	}
}

//Message loop 안에 들어있는 Frame
bool GraphicsClass::Frame()
{
	bool result = false;
	//연산 수행
	//---------------------------------------

	//---------------------------------------
	//그래픽 렌더링 수행
	result = Render();
	if (!result) 
		return false;

	return Render();
}


bool GraphicsClass::Render()
{
	//씬 그리기를 시작하기 위해 버퍼의 내용을 지움
	_D3DC->BeginScene(1.0f, 1.0f, 0.0f, 1.0f); //화면 Color 표시

	//화면에 그림을 그리는 Rendering 작업
	//Camera -> 그릴 대상에 대한 정점 정보

	//버퍼에 그려진 Scene을 화면에 표시
	_D3DC->EndScene();

	return true;
}