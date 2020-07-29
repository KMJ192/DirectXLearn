#include"Stdafx.h"
#include "D3DClass.h"
#include "Camera.h"
#include "SpriteClass.h"
#include "ModelRectangle.h"
#include "ModelTriangle.h"
#include "ColorShader.h"
#include "TextureShader.h"
#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	_SDepth = 1000.0f;
	_SNear = 0.1f;
	_Vsync_enabled = true;
	_D3DC = NULL;

	_Camera = NULL;
	_Sprite = NULL;
	_ModelRectangle = NULL;
	_ModelTriangle = NULL;
	_ColorShader = NULL;
	_TextureShader = NULL;
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
	_Camera->SetPosition(0.0f, 0.0f, -7.0f);

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

	_ModelRectangle = new ModelRectangle;
	if (!_ModelRectangle)
	{
		MessageBox(hWnd, _T("ModelRectangle Error"), _T("Error"), MB_OK);
		return false;
	}

	result = _ModelRectangle->Initialize(_D3DC->GetDevice(), _T("Texture.jpg"));
	if (!result)
	{
		MessageBox(hWnd, _T("ModelRectangle Error"), _T("Error"), MB_OK);
		return false;
	}

	_Sprite = new SpriteClass;
	if (!_Sprite)
	{
		MessageBox(hWnd, _T("Sprite Error"), _T("Error"), MB_OK);
		return false;
	}

	result = _Sprite->Initialize(_D3DC->GetDevice(), 100, 200, _T("Texture.jpg"), 50, 50);
	if (!result)
	{
		MessageBox(hWnd, _T("_SpriteClass Error"), _T("Error"), MB_OK);
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

	_TextureShader = new TextureShader;
	if (!_TextureShader)
	{
		return _TextureShader;
	}

	result = _TextureShader->Initialize(_D3DC->GetDevice(), hWnd);
	if (!result)
	{
		MessageBox(hWnd, _T("Could not initialize the color textureshader object."), _T("Error"), MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Release()
{
	//Color ��ü ����
	if (_ColorShader)
	{
		_ColorShader->Release();
		delete _ColorShader;
		_ColorShader = NULL;
	}
	//Triangle��ü ����
	if (_ModelTriangle)
	{
		_ModelTriangle->Release();
		delete _ModelTriangle;
		_ModelTriangle = NULL;
	}
	
	if (_ModelRectangle)
	{
		_ModelRectangle->Release();
		delete _ModelRectangle;
		_ModelRectangle = NULL;
	}

	if (_Sprite)
	{
		_Sprite->Release();
		delete _Sprite;
		_Sprite = NULL;
	}

	//Camera��ü ����
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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result = false;

	//�� �׸��⸦ �����ϱ� ���� ������ ������ ����
	_D3DC->BeginScene(0.0f, 1.0f, 1.0f, 0.0f); //ȭ�� Color ǥ��

	//ȭ�鿡 �׸��� �׸��� Rendering �۾�
	//Camera -> �׸� ��� ���� ���� ����
	_Camera->Render();

	//ī�޶� �� d3d ��ü���� ����, �� �� ���� ��� ��������
	_Camera->GetViewMatrix(viewMatrix);
	_D3DC->GetWorldMatrix(worldMatrix);
	_D3DC->GetProjectionMatrix(projectionMatrix);
	_D3DC->GetOrthoMatrix(orthoMatrix)

	//Model Vertex �� IndexBuffer�� �׷��� ���������ο� ��ġ�Ͽ� ����� �غ�
	//_ModelTriangle->Render(_D3DC->GetDeviceContext());
	//_ModelRectangle->Render(_D3DC->GetDeviceContext());
	//_Sprite->Render(_D3DC, 10, 20);

	//Model Render
	/*result = _ColorShader->Render(_D3DC->GetDeviceContext(), 
		_ModelTriangle->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);*/

	/*result = _ColorShader->Render(_D3DC->GetDeviceContext(),
		_ModelRectangle->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);*/

	result = _TextureShader->Render(_D3DC->GetDeviceContext(), _ModelRectangle->GetIndexCount(), 
		worldMatrix, viewMatrix, projectionMatrix, _ModelRectangle->GetTexture());

	if (!result)
		return false;

	//���ۿ� �׷��� Scene�� ȭ�鿡 ǥ��
	_D3DC->EndScene();

	return true;
}