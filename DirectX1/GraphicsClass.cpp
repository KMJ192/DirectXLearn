#include"Stdafx.h"
#include "D3DClass.h"
#include "Camera.h"
//#include "ModelRectangle.h"
//#include "ModelTriangle.h"
#include "ColorShader.h"
#include "TextureShader.h"
#include "GraphicsClass.h"
#include "SpriteObjectClass.h"

GraphicsClass::GraphicsClass()
{
	_SDepth = 1000.0f;
	_SNear = 0.1f;
	_D3DC = NULL;
	_Vsync_enabled = true;

	_Camera = NULL;
	_ColorShader = NULL;
	_TextureShader = NULL;

	_SpriteObj = NULL;
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

	_SpriteObj = new SpriteObjectClass;
	if (!_SpriteObj)
	{
		MessageBox(hWnd, _T("Sprite Error"), _T("Error"), MB_OK);
		return false;
	}

	result = _SpriteObj->Initialize(hWnd, _D3DC->GetDevice(), screenW, screenH, 100, 100, L"Data\\Texture.jpg");
	if (!result)
	{
		MessageBox(hWnd, _T("_SpriteClass Error"), _T("Error"), MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Release()
{
	if (_SpriteObj)
	{
		_SpriteObj->Release();
		delete _SpriteObj;
		_SpriteObj = NULL;
	}

	if (_TextureShader)
	{
		_TextureShader->Release();
		delete _TextureShader;
		_TextureShader = NULL;
	}

	//Color 객체 해제
	if (_ColorShader)
	{
		_ColorShader->Release();
		delete _ColorShader;
		_ColorShader = NULL;
	}
	
	//Camera객체 해제
	if (_Camera)
	{
		delete _Camera;
		_Camera = NULL;
	}

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


	//그래픽 렌더링 수행
	result = Render();
	if (!result) 
		return false;

	return Render();
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result = false;

	//씬 그리기를 시작하기 위해 버퍼의 내용을 지움
	_D3DC->BeginScene(0.0f, 1.0f, 1.0f, 0.0f); //화면 Color 표시

	//화면에 그림을 그리는 Rendering 작업
	//Camera -> 그릴 대상에 대한 정점 정보
	_Camera->Render();

	//카메라 및 d3d 객체에서 세계, 뷰 및 투영 행렬 가져오기
	_Camera->GetViewMatrix(viewMatrix);
	_D3DC->GetWorldMatrix(worldMatrix);
	_D3DC->GetProjectionMatrix(projectionMatrix);
	_D3DC->GetOrthoMatrix(orthoMatrix);


	if (!result)
		return false;

	_D3DC->TurnZBufferOff();

	_SpriteObj->Render(_D3DC, worldMatrix, viewMatrix, orthoMatrix, _TextureShader);

	_D3DC->TurnZBufferOn();

	_D3DC->EndScene();

	//버퍼에 그려진 Scene을 화면에 표시
	_D3DC->EndScene();

	return true;
}