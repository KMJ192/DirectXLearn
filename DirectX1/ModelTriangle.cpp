#include "Stdafx.h"
#include "ModelTriangle.h"

ModelTriangle::ModelTriangle()
{
	_VertexBuffer = NULL;
	_IndexBuffer = NULL;
}

ModelTriangle::ModelTriangle(const ModelTriangle& other)
{
}

ModelTriangle::~ModelTriangle()
{
}

bool ModelTriangle::Initialize(ID3D11Device* device)
{
	if (!InitializeBuffers(device))
		return false;

	return true;
}

void ModelTriangle::Release()
{
	//���� ���ۿ� �ε��� ���۸� ����
	ReleaseBuffers();
}

void ModelTriangle::Render(ID3D11DeviceContext* deviceContext)
{
	//���� ���ۿ� �ε��� ���۸� �׷��Ƚ� ������ ���ο� �־� ȭ�鿡 �׸� �غ�
	RenderBuffers(deviceContext);
}

int ModelTriangle::GetIndexCount()
{
	return _IndexCount;
}

bool ModelTriangle::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	//���� �迭�� ���̸� ����
	_VertexCount = 3;
	//�ε��� �迭�� ���� ����
	_IndexCount = 3;
	//���� �迭�� ����
	vertices = new VertexType[_VertexCount];
	if (!vertices) return false;
	//�ε��� �迭 ����
	indices = new unsigned long[_IndexCount];
	if (!indices) return false;

	//���� �迭�� ���� ����
	vertices[0]._Position = XMFLOAT3(-1.0f, -1.0f, -1.0f); //���� �Ʒ�
	vertices[0]._Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1]._Position = XMFLOAT3(0.0f, 1.0f, 0.0f); //��� ���
	vertices[1]._Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2]._Position = XMFLOAT3(1.0f, -1.0f, -1.0f); //������ �Ʒ�
	vertices[2]._Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	indices[0] = 0;//���� �Ʒ�
	indices[1] = 1;//��� ���
	indices[2] = 2;//������ �Ʒ�
	
	//���� ������ Description �ۼ�
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * _VertexCount;
	vertexBufferDesc.BindFlags = 0;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	
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

void ModelTriangle::ReleaseBuffers()
{
	//�ε��� ���۸� ����
	if (_IndexBuffer)
	{
		_IndexBuffer->Release();
		_IndexBuffer = NULL;
	}
	//���� ���۸� ����
	if (_VertexBuffer)
	{
		_VertexBuffer->Release();
		_VertexBuffer = NULL;
	}

	return;
}

void ModelTriangle::RenderBuffers(ID3D11DeviceContext* deviceContext)
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