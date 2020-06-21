#pragma once

class ModelTriangle
{
	struct VertexType {
		XMFLOAT3 _Position;
		XMFLOAT4 _Color;
	};

	ID3D11Buffer* _VertexBuffer;
	ID3D11Buffer* _IndexBuffer;
	int _VertexCount;
	int _IndexCount;

public:
	ModelTriangle();
	ModelTriangle(const ModelTriangle&);
	~ModelTriangle();

	bool Initialize(ID3D11Device*);
	void Release();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	
private:
	bool InitializeBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

};