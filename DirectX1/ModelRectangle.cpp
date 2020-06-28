#include "Stdafx.h"
#include "TextureClass.h"
#include "ModelRectangle.h"

ModelRectangle::ModelRectangle()
{
	_VertexBuffer = NULL;
	_IndexBuffer = NULL;
	_TextureC = NULL;
}

ModelRectangle::ModelRectangle(const ModelRectangle& other)
{
}

ModelRectangle::~ModelRectangle()
{
}

bool ModelRectangle::Initialize(ID3D11Device* device, const TCHAR* filename)
{
	//�ﰢ���� ���� Geomatri�� �����ϴ� ������ �� �ε��� ���� �ʱ�ȭ
	if (!InitializeBuffers(device)) 
		return false;

	//���� Texture Load
	if (!LoadTexture(device, filename))
		return false;

	return true;
}

void ModelRectangle::Release()
{
	ReleaseTexture();
	ReleaseBuffers();
}

void ModelRectangle::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int ModelRectangle::GetIndexCount()
{
	return _IndexCount;
}

ID3D11ShaderResourceView* ModelRectangle::GetTexture()
{
	return _TextureC->GetTexture();
}

bool ModelRectangle::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	//���� �迭�� ���̸� ����
	_VertexCount = 4;
	//�ε��� �迭�� ���� ����
	_IndexCount = 6;
	//���� �迭�� ����
	vertices = new VertexType[_VertexCount];
	if (!vertices) return false;
	//�ε��� �迭 ����
	indices = new unsigned long[_IndexCount];
	if (!indices) return false;

	//���� �迭�� ���� ����
	//���� �ϴ�
	vertices[0]._Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0]._Texture = XMFLOAT2(0.0f, 1.0f);
	vertices[0]._Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	//���� ���
	vertices[1]._Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[1]._Texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1]._Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	//������ ���
	vertices[2]._Position = XMFLOAT3(1.0f, -1.0f, 0.0f); 
	vertices[2]._Texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2]._Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	//���� �ϴ�
	vertices[3]._Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[3]._Texture = XMFLOAT2(1.0f, 0.0f);
	vertices[3]._Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);

	indices[0] = 0;//���� �Ʒ� 
	indices[1] = 1;//���� ���
	indices[2] = 2;//������ ���
	indices[3] = 2;//���� �Ʒ�
	indices[4] = 1;//������ �ϴ�
	indices[5] = 3;//������ ���

	//���� ������ Description �ۼ�
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * _VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//���� �����͸� ����Ű�� ���� ���ҽ� ����ü �ۼ�
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//���� ���� ����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_VertexBuffer);
	if (FAILED(result))
		return false;

	//�ε��� ������ Description �ۼ�
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//�ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ�
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//�ε��� ���� ����
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_IndexBuffer);
	if (FAILED(result))
		return false;

	//�����ǰ� ���� �Ҵ�� ���� ���ۿ� �ε��� ���� ����
	delete[] vertices;
	vertices = NULL;
	delete[] indices;
	indices = NULL;

	return true;
}

void ModelRectangle::ReleaseBuffers()
{
	if (_IndexBuffer) 
	{
		_IndexBuffer->Release();
		_IndexBuffer = NULL;
	}
	if (_VertexBuffer)
	{
		_VertexBuffer->Release();
		_VertexBuffer = NULL;
	}
	return;
}

void ModelRectangle::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;
	//���� ������ ������ �������� ����
	stride = sizeof(VertexType);
	offset = 0;

	//input assembler�� ���� ���۸� Ȱ��ȭ�Ͽ� �׷��� �� �ְ� ��.
	deviceContext->IASetVertexBuffers(0, 1, &_VertexBuffer, &stride, &offset);
	//input assembler�� �ε��� ���۸� Ȱ��ȭ �Ͽ� �׷��� �� �ְ� ��.
	deviceContext->IASetIndexBuffer(_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//���� ���۷� �׸� �⺻���� ����(3���� �׸���)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelRectangle::LoadTexture(ID3D11Device* device, const TCHAR* filename)
{
	bool result;

	//�ؽ��� ��ü �����
	_TextureC = new TextureClass;
	if (!_TextureC)
		return false;

	//�ؽ��� ������Ʈ �ʱ�ȭ
	result = _TextureC->Initialize(device, filename);
	if (!result) 
		return false;

	return true;
}

void ModelRectangle::ReleaseTexture()
{
	//�ؽ��� ��ü ����
	if (_TextureC)
	{
		_TextureC->Release();
		delete _TextureC;
		_TextureC = 0;
	}
	return;
}

