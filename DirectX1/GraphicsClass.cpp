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
	//D3D��ü ����
	if (_D3DC)
	{
		_D3DC->Release();
		delete _D3DC;
		_D3DC = NULL;
	}
}

//Message loop �ȿ� ����ִ� Frame
bool GraphicsClass::Frame()
{
	bool result = false;
	//���� ����
	//---------------------------------------

	//---------------------------------------
	//�׷��� ������ ����
	result = Render();
	if (!result) 
		return false;

	return Render();
}


bool GraphicsClass::Render()
{
	//�� �׸��⸦ �����ϱ� ���� ������ ������ ����
	_D3DC->BeginScene(1.0f, 1.0f, 0.0f, 1.0f); //ȭ�� Color ǥ��

	//ȭ�鿡 �׸��� �׸��� Rendering �۾�
	//Camera -> �׸� ��� ���� ���� ����

	//���ۿ� �׷��� Scene�� ȭ�鿡 ǥ��
	_D3DC->EndScene();

	return true;
}