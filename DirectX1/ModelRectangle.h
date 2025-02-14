#pragma once

class TextureClass;

class ModelRectangle 
{
	struct VertexType {
		XMFLOAT3 _Position;
		XMFLOAT2 _Texture;
		XMFLOAT4 _Color;
	};

	ID3D11Buffer* _VertexBuffer;
	ID3D11Buffer* _IndexBuffer;
	int _VertexCount;
	int _IndexCount;
	TextureClass* _TextureC;
	

public:
	ModelRectangle();
	ModelRectangle(const ModelRectangle&);
	~ModelRectangle();

	bool Initialize(ID3D11Device*, const TCHAR*);
	void Release();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, const TCHAR*);
	void ReleaseTexture();

};