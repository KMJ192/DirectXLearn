#include "Stdafx.h"
#include "TextureClass.h"
#include "SpriteClass.h"

SpriteClass::SpriteClass()
{
	_VertexBuffer = NULL;
	_IndexBuffer = NULL;
	_Texture = NULL;
}

SpriteClass::SpriteClass(const SpriteClass& other)
{
}

SpriteClass::~SpriteClass()
{
}

bool SpriteClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, const TCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	//ȭ�� ũ�� ����
	_ScreenW = screenWidth;
	_ScreenH = screenHeight;

	//�� ��Ʈ ���� ������ �� �ȼ� ũ�� ����
	_BitmapW = bitmapWidth;
	_BitmapH = bitmapHeight;

	//���� ������ ��ġ�� ������ �ʱ�ȭ
	_PreviousPosX = -1;
	_PreviousPosY = -1;

	//���ؽ� �� �ε��� ���� �ʱ�ȭ
	result = InitializeBuffers(device);
	if (!result)
		return false;

	//���� �ؽ��ķε�
	result = LoadTexture(device, textureFilename);
	if (!result)
		return false;

	return true;
}

void SpriteClass::Release()
{
	//�� �ؽ��� ����
	ReleaseTexture();

	//���ؽ� �� �ε��� ���� ����
	ReleaseBuffers();
}

bool SpriteClass::Render(ID3D11DeviceContext* deviceContext, float positionX, float positionY, float sizeU, float sizeV, int posU, int posV)
{
	bool result;

	//���� ���� ���۸� �ٽ� �����Ͽ� ȭ���� �ٸ� ��ġ�� ������ ����
	result = UpdateBuffers(deviceContext, positionX, positionY, sizeU, sizeV, posU, posV);
	if (!result)
		return false;

	//�׷��� ������ ���ο� ������ �� �ε��� ���۸� ��ġ�Ͽ� �׸��� �غ�
	RenderBuffers(deviceContext);

	return true;
}

int SpriteClass::GetIndexCount()
{
	return _IndexCount;
}

ID3D11ShaderResourceView* SpriteClass::GetTexture()
{
	return _Texture->GetTexture();
}

bool SpriteClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;
	
	//���� �迭�� ���� ���� ����
	_VertexCount = 6;

	//�ε��� �迭�� �ε��� �� ����
	_IndexCount = _VertexCount;

	//���ؽ� �迭 �����
	vertices = new VertexType[_VertexCount];
	if (!vertices)
		return false;

	//�ε��� �迭 �����
	indices = new unsigned long[_IndexCount];
	if (!indices)
		return false;

	//���� ���� �迭�� 0���� �ʱ�ȭ
	memset(vertices, 0, (sizeof(VertexType) * _VertexCount));

	for (i = 0; i < _IndexCount; i++) {
		indices[i] = i;
	}

	//���� ���� ������ ������ ����
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * _VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//���� ���ҽ� ������ ���� �����Ϳ� ���� ������ �ο�
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//���ؽ� ���۸� ����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_VertexBuffer);

	if (FAILED(result))
		return false;

	//���� �ε��� ���ۿ� ���� ���� ����.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� ������
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//�ε��� ���� �����
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_IndexBuffer);
	if (FAILED(result))
		return false;

	//���ؽ��� �ε��� ���۰� �����ǰ� �ε� �� ���¿����� �迭�� ����
	delete[] vertices;
	vertices = NULL;
	delete[] indices;
	indices = NULL;

	return true;
}

void SpriteClass::ReleaseBuffers()
{
	//�ε��� ���� ����
	if (_IndexBuffer)
	{
		_IndexBuffer->Release();
		_IndexBuffer = NULL;
	}

	//���ؽ� ���� ����
	if (_VertexBuffer)
	{
		_VertexBuffer->Release();
		_VertexBuffer = NULL;
	}
}

bool SpriteClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, float positionX, float positionY, float sizeU, float sizeV, int posU, int posV)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	//�� ��Ʈ ���� ������ �� ��ġ�� ������� ���� ��� ���� �ùٸ� ���� ������ �����Ƿ� ���� ���۸� ������Ʈ
	//if ((positionX == _PreviousPosX) && (positionY == _PreviousPosY))
	//	return true;

	//����� ��� ������ �Ǵ� ��ġ ������Ʈ
	_PreviousPosX = positionX;
	_PreviousPosY = positionY;

	//��Ʈ ���� ���ʸ��� ȭ�� ��ǥ ���
	left = (float)((_ScreenW / 2) * -1) + (float)positionX;
	//��Ʈ �� �������� ȭ�� ��ǥ�� ���
	right = left + (float)_BitmapW;
	//��Ʈ �� ����� ȭ�� ��ǥ�� ���
	top = (float)(_ScreenH / 2) + (float)positionY;
	//��Ʈ ���� �Ʒ����� ȭ�� ��ǥ ���
	bottom = top - (float)_BitmapH;

	//���ؽ� �迭 �����
	vertices = new VertexType[_VertexCount];
	if (!vertices)
		return false;

	//���ؽ� �迭�� ������ �ε�
	//ù��° 3����
	//vertices[0].position = XMFLOAT3(left, top, 0.0f); // Top left
	//vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	//vertices[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//vertices[1].position = XMFLOAT3(right, bottom, 0.0f); // Bottom right
	//vertices[1].texture = XMFLOAT2(1.0f, 1.0f);
	//vertices[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//vertices[2].position = XMFLOAT3(left, bottom, 0.0f); // Bottom left
	//vertices[2].texture = XMFLOAT2(0.0f, 1.0f);
	//vertices[2].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//vertices[3].position = XMFLOAT3(left, top, 0.0f); // Top left
	//vertices[3].texture = XMFLOAT2(0.0f, 0.0f);
	//vertices[3].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//vertices[4].position = XMFLOAT3(right, top, 0.0f); // Top rigth
	//vertices[4].texture = XMFLOAT2(1.0f, 0.0f);
	//vertices[4].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//vertices[5].position = XMFLOAT3(right, bottom, 0.0f); // Bottom rigth
	//vertices[5].texture = XMFLOAT2(1.0f, 1.0f);
	//vertices[5].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	vertices[0].position = XMFLOAT3(left, bottom, 0.0f);
	vertices[0].texture = XMFLOAT2((posU + 1) * sizeU, (posV + 1) * sizeV);
	vertices[0].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[1].position = XMFLOAT3(left, top, 0.0f);
	vertices[1].texture = XMFLOAT2((posU + 1) * sizeU, posV * sizeV);
	vertices[1].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[2].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[2].texture = XMFLOAT2(posU  * sizeU, (posV + 1) * sizeV);
	vertices[2].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[3].position = XMFLOAT3(right, top, 0.0f);
	vertices[3].texture = XMFLOAT2(posU * sizeU, posV * sizeV);
	vertices[3].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//���ؽ� ���۸� �� �� �ֵ��� ���
	result = deviceContext->Map(_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	//���ؽ� ������ �����Ϳ� ���� ������ ��������
	verticesPtr = (VertexType*)mappedResource.pData;

	//�����͸� ���� ���ۿ� ����
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * _VertexCount));

	//���ؽ� ���� ��� ����
	deviceContext->Unmap(_VertexBuffer, 0);

	//���ؽ� �迭�� �� �̻� �ʿ����� �ʰ� ����
	delete[] vertices;
	vertices = NULL;

	return true;
}

void SpriteClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//���� ���� ���� �� ������ ����
	stride = sizeof(VertexType);
	offset = 0;

	//������ �� �� �յ��� �Է� ��������� ���� ���۸� Ȱ������ ����
	deviceContext->IASetVertexBuffers(0, 1, &_VertexBuffer, &stride, &offset);

	//������ �� �� �յ��� �Է� ���������� �ε��� ���۸� Ȱ������ ����
	deviceContext->IASetIndexBuffer(_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//�� ���� ���ۿ��� ������ �Ǿ�� �ϴ� ������Ƽ�� ���� ����
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

bool SpriteClass::LoadTexture(ID3D11Device* device, const TCHAR* filename)
{
	bool result;

	//�ؽ��� ��ü ����� 
	_Texture = new TextureClass;
	if (!_Texture)
		return false;

	//�ؽ��� ������Ʈ�� �ʱ�ȭ
	result = _Texture->Initialize(device, filename);
	if (!result)
		return false;

	return true;
}

void SpriteClass::ReleaseTexture()
{
	//�ؽ��� ��ü ����
	if (_Texture)
	{
		_Texture->Release();
		delete _Texture;
		_Texture = NULL;
	}
}