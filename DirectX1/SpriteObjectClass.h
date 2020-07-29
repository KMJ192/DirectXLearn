#pragma once

class D3DClass;
class TextureShader;
class SpriteClass;

class SpriteObjectClass
{
	float _MoveSpeed;
	float _PosX;
	float _PosY;
	float _SizeU;
	float _SizeV;
	int _PosU;
	int _PosV;
	float _CheckTime;

	SpriteClass* _Model;

public:
	SpriteObjectClass();
	SpriteObjectClass(const SpriteObjectClass&);
	~SpriteObjectClass();

	bool Initialize(HWND, ID3D11Device*, int, int, int, int);
	void Release();
	bool Process(float, float, float);
	bool Render(D3DClass*, XMMATRIX, XMMATRIX, XMMATRIX, TextureShader*);

};

