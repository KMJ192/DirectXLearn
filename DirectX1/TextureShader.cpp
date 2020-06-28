#include "Stdafx.h"
#include "D3DUtilClass.h"
#include "TextureShader.h"

TextureShader::TextureShader()
{
	_VertexShader = NULL;
	_PixelShader = NULL;
	_Layout = NULL;
	_MatrixBuffer = NULL;
	_SampleState = NULL;
}

TextureShader::TextureShader(const TextureShader& other)
{
}

TextureShader::~TextureShader()
{
}

bool TextureShader::Initialize(ID3D11Device* device, HWND hWnd)
{
	bool result;
	result = InitShader(device, hWnd, _T("Texture.vs"), _T("Texture.ps"));

	if (!result) return false;

	return true;
}

void TextureShader::Release()
{
	//���� ���̴��� �ȼ� �����̴� �� �׿� ���õ� �͵��� ��ȯ
	ReleaseShader();
}

bool TextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldM, XMMATRIX viewM, XMMATRIX projectionM, ID3D11ShaderResourceView* texture)
{
	bool result;
	result = SetShaderParameters(deviceContext, worldM, viewM, projectionM, texture);
	if (!result)
		return false;

	//���̴��� �̿��Ͽ� �غ�� ���۸� �׸�
	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShader::InitShader(ID3D11Device* device, HWND hWnd, const TCHAR* vsFilename, const TCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	//����ϴ� Pointer�� null ����
	errorMessage = NULL;
	vertexShaderBuffer = NULL;
	pixelShaderBuffer = NULL;

	//���� ���̴��� ������ ��
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, NULL, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hWnd, vsFilename);
		else
			MessageBox(hWnd, vsFilename, L"Missing Shader File", MB_OK);
		
		return false;
	}


	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, NULL, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hWnd, vsFilename);
		else
			MessageBox(hWnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	//���۷κ��� ���� ���̴��� ����
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &_VertexShader);
	if (FAILED(result))
		return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &_PixelShader);
	if (FAILED(result))
		return false;

	//���� �Է� ���̾ƿ� description �ۼ�
	//ModelClass�� Shader�� �ִ� vertexType�� ��ġ�ؾ� ��
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;
	
	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "COLOR";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	//���̾ƿ��� ��� ������ ������
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	//���� �Է� ���̾ƿ��� ����
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &_Layout);

	if (FAILED(result))
		return false;

	//���̻� ������ �ʴ� ���� ���̴� ���ۿ� �ȼ� ���̴� ���۸� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//���� ���̴��� �ִ� �۷� ��� ������ description�� �ۼ�
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� ��
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &_MatrixBuffer);
	if (FAILED(result))
		return true;

	//Create a texture sampler state descrition
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	//�ؽ��� ���÷� ���� �����
	result = device->CreateSamplerState(&samplerDesc, &_SampleState);
	if (FAILED(result))
		return false;
	
	return true;
}

void TextureShader::ReleaseShader()
{
	if (_SampleState)
	{
		_SampleState->Release();
		_SampleState = 0;
	}

	//��� ���� ����
	if (_MatrixBuffer) 
	{
		_MatrixBuffer->Release();
		_MatrixBuffer = 0;
	}

	//���̾ƿ� ����
	if (_Layout)
	{
		_Layout->Release();
		_Layout = 0;
	}

	if (_PixelShader)
	{
		_PixelShader->Release();
		_PixelShader = 0;
	}

	if (_VertexShader)
	{
		_VertexShader->Release();
		_VertexShader = 0;
	}
}

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldM, XMMATRIX viewM, XMMATRIX projectionM, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//����� transpose�Ͽ� ���̴����� ����� �� �ְ� ��
	worldM = XMMatrixTranspose(worldM);
	viewM = XMMatrixTranspose(viewM);
	projectionM = XMMatrixTranspose(projectionM);

	//��� ������ ������ �� �� �ֵ��� ���
	result = deviceContext->Map(_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	//��� ������ �����Ϳ� ���� �����͸� ������
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//��� ���ۿ� ����� ������
	//dataPtr = (MatrixBufferType*)mappedResource.pData;

	//��� ���ۿ� ����� ����
	dataPtr->world = worldM;
	dataPtr->view = viewM;
	dataPtr->projection = projectionM;

	//��� ������ ��� ����
	deviceContext->Unmap(_MatrixBuffer, 0);

	//���� ���̴������� ��� ������ ��ġ�� ����
	bufferNumber = 0;

	//���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ�
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &_MatrixBuffer);

	//Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(_Layout);

	deviceContext->VSSetShader(_VertexShader, NULL, 0);
	deviceContext->PSSetShader(_PixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &_SampleState);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}