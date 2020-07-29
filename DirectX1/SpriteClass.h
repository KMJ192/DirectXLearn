#pragma once

class TextureClass;

class SpriteClass
{
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT4 color;
	};

	ID3D11Buffer *_VertexBuffer;
	ID3D11Buffer *_IndexBuffer;
	int _VertexCount;
	int _IndexCount;
	TextureClass* _Texture;
	int _ScreenW;
	int _ScreenH;
	int _BitmapW;
	int _BitmapH;
	int _PreviousPosX;
	int _PreviousPosY;

	bool InitializeBuffers(ID3D11Device*);
	void ReleaseBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, float, float, float, float, int, int);
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, const TCHAR*);
	void ReleaseTexture();

public:
	SpriteClass();
	SpriteClass(const SpriteClass&);
	~SpriteClass();

	bool Initialize(ID3D11Device*, int, int, const TCHAR*, int, int);
	void Release();
	bool Render(ID3D11DeviceContext*, float, float, float, float, int, int);
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
};

