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

	//화면 크기 저장
	_ScreenW = screenWidth;
	_ScreenH = screenHeight;

	//이 비트 맵을 랜더링 할 픽셀 크기 저장
	_BitmapW = bitmapWidth;
	_BitmapH = bitmapHeight;

	//이전 랜더링 위치를 음수로 초기화
	_PreviousPosX = -1;
	_PreviousPosY = -1;

	//버텍스 및 인덱스 버퍼 초기화
	result = InitializeBuffers(device);
	if (!result)
		return false;

	//모델의 텍스쳐로드
	result = LoadTexture(device, textureFilename);
	if (!result)
		return false;

	return true;
}

void SpriteClass::Release()
{
	//모델 텍스쳐 해제
	ReleaseTexture();

	//버텍스 및 인덱스 버퍼 해제
	ReleaseBuffers();
}

bool SpriteClass::Render(ID3D11DeviceContext* deviceContext, float positionX, float positionY, float sizeU, float sizeV, int posU, int posV)
{
	bool result;

	//동적 정점 버퍼를 다시 빌드하여 화면의 다른 위치로 렌더링 가능
	result = UpdateBuffers(deviceContext, positionX, positionY, sizeU, sizeV, posU, posV);
	if (!result)
		return false;

	//그래픽 파이프 라인에 꼭지점 및 인덱스 버퍼를 배치하여 그리기 준비
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
	
	//정점 배열의 정점 수를 설정
	_VertexCount = 6;

	//인덱스 배열의 인덱스 수 설정
	_IndexCount = _VertexCount;

	//버텍스 배열 만들기
	vertices = new VertexType[_VertexCount];
	if (!vertices)
		return false;

	//인덱스 배열 만들기
	indices = new unsigned long[_IndexCount];
	if (!indices)
		return false;

	//먼저 정점 배열을 0으로 초기화
	memset(vertices, 0, (sizeof(VertexType) * _VertexCount));

	for (i = 0; i < _IndexCount; i++) {
		indices[i] = i;
	}

	//정적 정점 버퍼의 설명을 설정
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * _VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//하위 리소스 구조에 정점 데이터에 대한 포인터 부여
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//버텍스 버퍼를 만듬
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_VertexBuffer);

	if (FAILED(result))
		return false;

	//정점 인덱스 버퍼에 대한 설명 설정.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//하위 리소스 구조에 인덱스 데이터에 대한 포인터를 지정함
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//인덱스 버퍼 만들기
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_IndexBuffer);
	if (FAILED(result))
		return false;

	//버텍스와 인덱스 버퍼가 생성되고 로드 된 상태에서의 배열을 놓음
	delete[] vertices;
	vertices = NULL;
	delete[] indices;
	indices = NULL;

	return true;
}

void SpriteClass::ReleaseBuffers()
{
	//인덱스 버퍼 해제
	if (_IndexBuffer)
	{
		_IndexBuffer->Release();
		_IndexBuffer = NULL;
	}

	//버텍스 버퍼 해제
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

	//이 비트 맵을 렌더링 할 위치가 변경되지 않은 경우 현재 올바른 마게 변수가 있으므로 정점 버퍼를 업데이트
	//if ((positionX == _PreviousPosX) && (positionY == _PreviousPosY))
	//	return true;

	//변경된 경우 렌더링 되는 위치 업데이트
	_PreviousPosX = positionX;
	_PreviousPosY = positionY;

	//비트 맵의 왼쪽면의 화면 좌표 계산
	left = (float)((_ScreenW / 2) * -1) + (float)positionX;
	//비트 맵 오른쪽의 화면 좌표를 계산
	right = left + (float)_BitmapW;
	//비트 맵 상단의 화면 좌표를 계산
	top = (float)(_ScreenH / 2) + (float)positionY;
	//비트 맵의 아래쪽의 화면 좌표 계산
	bottom = top - (float)_BitmapH;

	//버텍스 배열 만들기
	vertices = new VertexType[_VertexCount];
	if (!vertices)
		return false;

	//버텍스 배열에 데이터 로드
	//첫번째 3각형
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

	//버텍스 버퍼를 쓸 수 있도록 잠금
	result = deviceContext->Map(_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	//버텍스 버퍼의 데이터에 대한 포인터 가져오기
	verticesPtr = (VertexType*)mappedResource.pData;

	//데이터를 정점 버퍼에 복사
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * _VertexCount));

	//버텍스 버퍼 잠금 해제
	deviceContext->Unmap(_VertexBuffer, 0);

	//버텍스 배열을 더 이상 필요하지 않게 해제
	delete[] vertices;
	vertices = NULL;

	return true;
}

void SpriteClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//정점 버퍼 보폭 및 오프셋 설정
	stride = sizeof(VertexType);
	offset = 0;

	//렌더링 할 수 잇도록 입력 어셈블리에서 정점 버퍼를 활성으로 설정
	deviceContext->IASetVertexBuffers(0, 1, &_VertexBuffer, &stride, &offset);

	//렌더링 될 수 잇도록 입력 업셈블러에서 인덱스 버퍼를 활성으로 설정
	deviceContext->IASetIndexBuffer(_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//이 정점 버퍼에서 렌더링 되어야 하는 프리미티브 유형 설정
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

bool SpriteClass::LoadTexture(ID3D11Device* device, const TCHAR* filename)
{
	bool result;

	//텍스쳐 객체 만들기 
	_Texture = new TextureClass;
	if (!_Texture)
		return false;

	//텍스쳐 오브젝트를 초기화
	result = _Texture->Initialize(device, filename);
	if (!result)
		return false;

	return true;
}

void SpriteClass::ReleaseTexture()
{
	//텍스쳐 객체 해제
	if (_Texture)
	{
		_Texture->Release();
		delete _Texture;
		_Texture = NULL;
	}
}