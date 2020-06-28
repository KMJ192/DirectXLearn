#pragma once

class TextureClass
{
	ID3D11ShaderResourceView* _Texture;
	HRESULT CreateShaderResourceViewFromFile(ID3D11Device*, const TCHAR*, const TCHAR*);

public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, const TCHAR*);
	void Release();

	ID3D11ShaderResourceView* GetTexture();

};

