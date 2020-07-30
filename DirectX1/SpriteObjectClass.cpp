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
	_SizeU = 0.4; //ĳ���� �̹����� �°� ����
	_SizeV = 0.4; //ĳ���� �̹����� �°� ����
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
	//��Ʈ �� ���ؽ� �� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ�
	if (!_Model->Render(d3dClass->GetDeviceContext(), _PosX, _PosY, _SizeU, _SizeV, _PosU, _PosV))
		return false;
	
	//���� ȿ���� ���� -> ���ĺ��� On
	d3dClass->EnableAlphaBlending();

	//���̴��� ����Ͽ� �� ������
	if (!pTexShader->Render(d3dClass->GetDeviceContext(), _Model->GetIndexCount(), worldM, viewM, orthoM, _Model->GetTexture()))
		return false;

	//���� ���� off
	d3dClass->DisableAlphaBlending();

	return true;
}
