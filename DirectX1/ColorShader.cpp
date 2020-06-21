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
	//���� ���̴��� �ȼ� ���̴� �ʱ�ȭ
	result = InitShader(device, hWnd, _T("ShaderData\\color.vs"), _T("ShaderData\\color.ps"));
	if (!result)
		return false;

	return true;
}

void ColorShader::Release()
{
	//����, �ȼ� ���̴� ���� ��ȯ
	ReleaseShader();
}

bool ColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldM, XMMATRIX viewM, XMMATRIX projectionM)
{
	bool result;
	result = SetShaderParameters(deviceContext, worldM, viewM, projectionM);
	if (!result)
		return false;
	
	//���̴��� �̿��Ͽ� �غ�� ���� �׸�
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

	//�� �Լ����� ����ϴ� Pointer Null�� ����
	errorMessage = NULL;
	vertexShaderBuffer = NULL;
	pixelShaderBuffer = NULL;

	//���� ���̴� ������
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, NULL, &vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		//���̴��� �����Ͽ� �����ϸ� ErrorMessage ���
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hWnd, vsFilename);
		else //vs������ ���� ���
			MessageBox(hWnd, vsFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	//�ȼ� ���̴��� ������ ��
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, NULL, &pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		//���̴��� �����Ͽ� �����ϸ� ErrorMessage ���
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hWnd, psFilename);
		else //ps������ ���� ���
			MessageBox(hWnd, psFilename, L"Missing Shader File", MB_OK);

		return false;
	}

	//���۷κ��� ���� ���̴��� ����
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &_VertexShader);

	if (FAILED(result))
		return false;

	//���۷� ���� �ȼ� ���̴��� ����
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &_PixelShader);
	
	if (FAILED(result))
		return false;

	//���� �Է� ���̾ƿ� description�� �ۼ�
	//ModeClass�� ���̴��� �ִ� VertexType�� ��ġ�Ͽ��� ��
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

	//���̾ƿ��� ��� ������ ������
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	//���� �Է� ���̾ƿ��� ����
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &_Layout);
	
	if (FAILED(result))
		return false;

	//���̻� ������ �ʴ� ���� ���̴� �ǹ��� �ȼ� ���̴� ���۸� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//���� ���̴��� �ִ� ��� ��� ������ desciption�� �ۼ�
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� ��
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &_MatrixBuffer);
	if (FAILED(result))
		return false;

	return true;
}

void ColorShader::ReleaseShader()
{
	//��� ���۸� ����
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

	//�ȼ� ���̴� ����
	if (_PixelShader)
	{
		_PixelShader->Release();
		_PixelShader = 0;
	}

	//vertex ���̴� ����
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

	//��� ���ۿ� ����� ����
	dataPtr->world = worldM;
	dataPtr->view = viewM;
	dataPtr->projection = projectionM;

	//��� ������ ����� ����
	deviceContext->Unmap(_MatrixBuffer, 0);

	//���� ���̴������� ��� ������ ��ġ�� ����
	bufferNumber = 0;

	//���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ�
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &_MatrixBuffer);

	return true;
}

void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//���� �Է� ���̾ƿ��� ����
	deviceContext->IASetInputLayout(_Layout);

	//�ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� ����
	deviceContext->VSSetShader(_VertexShader, NULL, 0);
	deviceContext->PSSetShader(_PixelShader, NULL, 0);

	//�ﰢ���� �׸�
	deviceContext->DrawIndexed(indexCount, 0, 0);
}