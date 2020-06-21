#include"Stdafx.h"
#include "D3DClass.h"
#include "Camera.h"
#include "ModelTriangle.h"
#include "ColorShader.h"
#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	_SDepth = 1000.0f;
	_SNear = 0.1f;
	_Vsync_enabled = true;
	_D3DC = NULL;
	_Camera = NULL;
	_ModelTriangle = NULL;
	_ColorShader = NULL;
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
	_Camera = new Camera;
	if (!_Camera) 
	{
		MessageBox(hWnd, _T("Camera Error"), _T("Error"), MB_OK);
		return false;
	}
	_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	_ModelTriangle = new ModelTriangle;
	if (!_ModelTriangle)
	{
		MessageBox(hWnd, _T("ModelTriangle Error"), _T("Error"), MB_OK);
		return false;
	}

	result = _ModelTriangle->Initialize(_D3DC->GetDevice());
	if (!result)
	{
		MessageBox(hWnd, _T("ModelTriangle Error"), _T("Error"), MB_OK);
		return false;
	}
	_ColorShader = new ColorShader;
	if (!_ColorShader)
	{
		MessageBox(hWnd, _T("ColorShader Error"), _T("Error"), MB_OK);
		return false;
	}

	result = _ColorShader->Initialize(_D3DC->GetDevice(), hWnd);
	if (!result)
	{
		MessageBox(hWnd, _T("ColorShader Error"), _T("Error"), MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Release()
{
	if (_ColorShader)
	{
		_ColorShader->Release();
		delete _ColorShader;
		_ColorShader = NULL;
	}

	if (_ModelTriangle)
	{
		_ModelTriangle->Release();
		delete _ModelTriangle;
		_ModelTriangle = NULL;
	}

	if (_Camera)
	{
		delete _Camera;
		_Camera = NULL;
	}

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
	_Camera->Render();
	XMMATRIX worldM, viewM, projectionM;
	_D3DC->GetWorldMatrix(worldM);
	_Camera->GetViewMatrix(viewM);
	_D3DC->GetProjectionMatrix(projectionM);

	_ModelTriangle->Render(_D3DC->GetDeviceContext());

	bool result;
	result = _ColorShader->Render(_D3DC->GetDeviceContext(), 
		_ModelTriangle->GetIndexCount(), worldM, viewM, projectionM);
	if (!result)
	{
		return false;
	}

	//���ۿ� �׷��� Scene�� ȭ�鿡 ǥ��
	_D3DC->EndScene();

	return true;
}