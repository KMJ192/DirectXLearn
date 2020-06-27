#include "Stdafx.h"
#include "ModelRectangle.h"

ModelRectangle::ModelRectangle()
{
	_VertexCount = NULL;
	_IndexCount = NULL;
}

ModelRectangle::ModelRectangle(const ModelRectangle& other)
{
}

ModelRectangle::~ModelRectangle()
{
}

bool ModelRectangle::Initialize(ID3D11Device* device)
{
	if (!InitializeBuffers(device)) 
		return false;

	return true;
}

void ModelRectangle::Release()
{
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

bool ModelRectangle::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	//정점 배열의 길이를 설정
	_VertexCount = 4;
	//인덱스 배열의 길이 설정
	_IndexCount = 6;
	//정점 배열을 생성
	vertices = new VertexType[_VertexCount];
	if (!vertices) return false;
	//인덱스 배열 생성
	indices = new unsigned long[_IndexCount];
	if (!indices) return false;

	//정점 배열에 값을 넣음
	//왼쪽 하단
	vertices[0]._Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0]._Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	//왼쪽 상단
	vertices[1]._Position = XMFLOAT3(1.0f, -1.0f, 0.0f); 
	vertices[1]._Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	//오른쪽 상단
	vertices[2]._Position = XMFLOAT3(-1.0f, 1.0f, 0.0f); 
	vertices[2]._Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	//왼쪽 하단
	vertices[3]._Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[3]._Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);

	indices[0] = 0;//왼쪽 아래 
	indices[1] = 2;//왼쪽 상단
	indices[2] = 1;//오른쪽 상단
	indices[3] = 1;//왼쪽 아래
	indices[4] = 2;//오른쪽 하단
	indices[5] = 3;//오른쪽 상단

	//정점 버퍼의 Description 작성
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * _VertexCount;
	vertexBufferDesc.BindFlags = 0;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	//정점 데이터를 가리키는 보조 리소스 구조체 작성
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//정점 버퍼 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_VertexBuffer);
	if (FAILED(result))
		return false;

	//인덱스 버퍼의 Description 작성
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//인덱스 데이터를 가리키는 보조 리소스 구조체를 작성
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//인덱스 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_IndexBuffer);
	if (FAILED(result))
		return false;

	//생성되고 값이 할당된 정점 버퍼와 인덱스 버퍼 해제
	delete[] vertices;
	vertices = NULL;
	delete[] indices;
	indices = NULL;

	return true;
}

void ModelRectangle::ReleaseBuffers()
{
	if (_IndexBuffer) {
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
	//정점 버퍼의 단위와 오프셋을 설정
	stride = sizeof(VertexType);
	offset = 0;

	//input assembler에 정점 버퍼를 활성화하여 그려질 수 있게 함.
	deviceContext->IASetVertexBuffers(0, 1, &_VertexBuffer, &stride, &offset);
	//input assembler에 인덱스 버퍼를 활성화 하여 그려질 수 있게 함.
	deviceContext->IASetIndexBuffer(_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//정점 버퍼로 그릴 기본형을 설정(3각형 그리기)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
