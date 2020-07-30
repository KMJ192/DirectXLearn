#include "Stdafx.h"
#include "D3DClass.h"
#include "TextureShader.h"
#include "SpriteClass.h"
#include "ObjectEnumHelper.h"
#include "SpriteObjectClass.h"

SpriteObjectClass::SpriteObjectClass()
{
	_MoveSpeed = 30.0f;
	_PosX = 0;
	_PosY = 0;
	_PosU = 3;
	_PosV = 1;
	_SizeU = 0.4; //캐릭터 이미지에 맞게 조정
	_SizeV = 0.4; //캐릭터 이미지에 맞게 조정
	_CheckTime = 0;
	_Model = NULL;
}

SpriteObjectClass::SpriteObjectClass(const SpriteObjectClass&)
{
}

SpriteObjectClass::~SpriteObjectClass()
{
}

bool SpriteObjectClass::Initialize(HWND, ID3D11Device*, int, int, int, int)
{
	return false;
}

void SpriteObjectClass::Release()
{
}

bool SpriteObjectClass::Process(float deltaTime, float mx, float my)
{
	_PosX += mx * deltaTime * _MoveSpeed;
	_PosY += my * deltaTime * _MoveSpeed;

	_CheckTime += deltaTime;
	if (_CheckTime >= 0.355f)
	{
		_CheckTime = 0;
		_PosU++;
		if (_PosU >= 5)
			_PosU = 3;
	}

	return true;
}

bool SpriteObjectClass::Render(D3DClass* d3dClass, XMMATRIX worldM, XMMATRIX viewM, XMMATRIX orthoM, TextureShader* pTexShader)
{
	//비트 맵 버텍스 및 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비
	if (!_Model->Render(d3dClass->GetDeviceContext(), _PosX, _PosY, _SizeU, _SizeV, _PosU, _PosV))
		return false;
	
	//투명 효과가 동작 -> 알파블렌딩 On
	d3dClass->EnableAlphaBlending();

	//셰이더를 사용하여 모델 렌더링
	if (!pTexShader->Render(d3dClass->GetDeviceContext(), _Model->GetIndexCount(), worldM, viewM, orthoM, _Model->GetTexture()))
		return false;

	//알파 블렌딩 off
	d3dClass->DisableAlphaBlending();

	return true;
}
