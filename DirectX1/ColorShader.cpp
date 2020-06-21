#include "Stdafx.h"
#include "D3DUtilClass.h"
#include "ColorShader.h"

ColorShader::ColorShader()
{
	_VertexShader = NULL;
	_PixelShader = NULL;
	_Layout = NULL;
	_MatrixBuffer = NULL;
}

ColorShader::ColorShader(const ColorShader& other)
{
}

ColorShader::~ColorShader()
{
}

bool ColorShader::Initialize(ID3D11Device* device, HWND hWnd)
{
	bool result;
	//정점 졔이더와 픽셀 셰이더 초기화
	result = InitShader(device, hWnd, _T("ShaderData\\color.vs"), _T("ShaderData\\color.ps"));
	if (!result)
		return false;

	return true;
}

void ColorShader::Release()
{
	//정점, 픽셀 셰이더 관련 반환
	ReleaseShader();
}

bool ColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldM, XMMATRIX viewM, XMMATRIX projectionM)
{
	bool result;
	result = SetShaderParameters(deviceContext, worldM, viewM, projectionM);
	if (!result)
		return false;
	
	//셰이더를 이용하여 준비된 버퍼 그림
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShader::InitShader(ID3D11Device* device, HWND hWnd, const TCHAR* vsFilename, const TCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	//이 함수에서 사용하는 Pointer Null로 설정
	errorMessage = NULL;
	vertexShaderBuffer = NULL;
	pixelShaderBuffer = NULL;

	//정점 셰이더 컴파일
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, NULL, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		//셰이더가 컴파일에 실패하면 ErrorMessage 기록
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hWnd, vsFilename);
		else //vs파일이 없는 경우
			MessageBox(hWnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	//픽셀 셰이더를 컴파일 함
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, NULL, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		//셰이더가 컴파일에 실패하면 ErrorMessage 기록
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hWnd, psFilename);
		else //ps파일이 없는 경우
			MessageBox(hWnd, psFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	//버퍼로부터 정점 셰이더를 생성
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &_VertexShader);

	if (FAILED(result))
		return false;

	//버퍼로 부터 픽셀 셰이더를 생성
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &_PixelShader);
	
	if (FAILED(result))
		return false;

	//정점 입력 레이아웃 description을 작성
	//ModeClass와 셰이더에 있는 VertexType과 일치하여야 함
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//레이아웃의 요소 갯수를 가져옴
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	//정점 입력 레이아웃을 생성
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &_Layout);
	
	if (FAILED(result))
		return false;

	//더이상 사용되지 않는 정점 셰이더 피버와 픽셀 셰이더 버퍼를 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//정점 셰이더에 있는 행렬 상수 버퍼의 desciption을 작성
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 점근할 수 있게 함
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &_MatrixBuffer);
	if (FAILED(result))
		return false;

	return true;
}

void ColorShader::ReleaseShader()
{
	//상수 버퍼를 해제
	if (_MatrixBuffer)
	{
		_MatrixBuffer->Release();
		_MatrixBuffer = 0;
	}

	//레이아웃 해제
	if (_Layout)
	{
		_Layout->Release();
		_Layout = 0;
	}

	//픽셀 셰이더 해제
	if (_PixelShader)
	{
		_PixelShader->Release();
		_PixelShader = 0;
	}

	//vertex 셰이더 해제
	if (_VertexShader)
	{
		_VertexShader->Release();
		_VertexShader = 0;
	}
}

bool ColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldM, XMMATRIX viewM, XMMATRIX projectionM)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//행렬을 transpose하여 셰이더에서 사용할 수 있게 함
	worldM = XMMatrixTranspose(worldM);
	viewM = XMMatrixTranspose(viewM);
	projectionM = XMMatrixTranspose(projectionM);

	//상수 버퍼의 내용을 쓸 수 있도록 잠금
	result = deviceContext->Map(_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	//상수 버퍼의 데이터에 대한 포인터를 가져옴
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//상수 버퍼에 행렬을 복사
	dataPtr->world = worldM;
	dataPtr->view = viewM;
	dataPtr->projection = projectionM;

	//상수 버퍼의 잠금을 해제
	deviceContext->Unmap(_MatrixBuffer, 0);

	//정점 셰이더에서의 상수 버퍼의 위치를 설정
	bufferNumber = 0;

	//정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿈
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &_MatrixBuffer);

	return true;
}

void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//정점 입력 레이아웃을 설정
	deviceContext->IASetInputLayout(_Layout);

	//삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정
	deviceContext->VSSetShader(_VertexShader, NULL, 0);
	deviceContext->PSSetShader(_PixelShader, NULL, 0);

	//삼각형을 그림
	deviceContext->DrawIndexed(indexCount, 0, 0);
}