#include "Stdafx.h"
#include "D3DClass.h"

D3DClass::D3DClass()
{
	_Device = NULL;
	_DeviceContext = NULL;
	_SwapChain = NULL;
	_RenderTargetView = NULL;
	_DepthStencilBuffer = NULL;
	_DepthStencilState = NULL;
	_DepthStencilView = NULL;
	_RasterState = NULL;

	_mEnable4xMsaa = false;
}

D3DClass::D3DClass(const D3DClass& other)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenW, int screenH, bool vSync, HWND hWnd, bool isFullScreen, float sDepth, float sNear)
{

	UINT m4xMsaaQuality = 0;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	//SwapChain description�� �ʱ�ȭ
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	_Vsync_enabled = vSync;

	//1. D3D11CreateDevice�Լ��� �̿��Ͽ� ID3D11Device �������̽��� ��ġ ����, ID3D11DeviceContext �������̽� ����
	if (!CreateDeviceNContext())
		return false;

	//2. ID3D11Device::CheckMultisampleQualityLevels�Լ��� �̿��Ͽ� 4XMSAAǰ�� ���� ���� ���� ����
	if (!CheckQualityLevels(&m4xMsaaQuality))
		return false;

	//3. ������ ��ȯ �罽�� Ư���� �����ϴ� DXGI_SWAP_CHAIN_DESC����ü�� ü��
	FillSwapChainSt(screenW, screenH, hWnd, isFullScreen, m4xMsaaQuality, &swapChainDesc);

	//4. ��ġ�� �����ϴµ� ����ߴ� IDXGIFactory �������̽��� �����ؼ� IDXGISwapChain�ν��Ͻ��� �����Ѵ�
	if (!CreateSwapChain(&swapChainDesc))
		return false;

	//5. ��ȯ �罽�� �ĸ� ���ۿ� ���� ���� ��� �並 ����
	CreateRenderTargetV();
	
	//6. ���� ���ٽ� ���ۿ� �׿� ����Ǵ� ����, ���Ľ� �並 ����
	if (!CreatedepthNStencil(screenW, screenH, m4xMsaaQuality))
		return false;

	//7. ���� ��� ��� ����, ���Ľ� �並 DirectX3D������� �� �ֵ��� ������ ������������ ��� ���ձ� �ܰ迡 ���´�
	if (!PiplineBindeing())
		return false;

	//8. ����Ʈ�� �����Ѵ�
	SettingViewPort(screenW, screenH, sDepth, sNear);

	return true;
}

void D3DClass::Release()
{
	if (_RasterState != NULL) {
		_RasterState->Release();
		_RasterState = NULL;
	}
	if (_DepthStencilView != NULL) {
		_DepthStencilView->Release();
		_DepthStencilView = NULL;
	}
	if (_DepthStencilState != NULL) {
		_DepthStencilState->Release();
		_DepthStencilState = NULL;
	}
	if (_DepthStencilBuffer != NULL) {
		_DepthStencilBuffer->Release();
		_DepthStencilBuffer = NULL;
	}
	if (_RenderTargetView != NULL) {
		_RenderTargetView->Release();
		_RenderTargetView = NULL;
	}
	if (_SwapChain != NULL) {
		_SwapChain->Release();
		_SwapChain = NULL;
	}
	if (_DeviceContext != NULL) {
		_DeviceContext->Release();
		_DeviceContext = NULL;
	}
	if (_Device != NULL) {
		_Device->Release();
	}
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	//���۸� � �������� ��������� ����
	float color[4] = {red, green, blue, alpha};

	//������� ������ ����
	_DeviceContext->ClearRenderTargetView(_RenderTargetView, color);

	//���� ���۸� ����
	_DeviceContext->ClearDepthStencilView(_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	//�������� �ϷεǾ����Ƿ� ������� ������ ȭ�鿡 ǥ��
	if (_Vsync_enabled) {
		//���ΰ�ħ ������ ����
		_SwapChain->Present(1, 0);
	}
	else {
		//������ �� ������ ǥ��
		_SwapChain->Present(0, 0);
	}
}

ID3D11Device* D3DClass::GetDevice()
{
	return _Device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return _DeviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& mat)
{
	mat = _ProjectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX& mat)
{
	mat = _WorldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& mat)
{
	mat = _OrthoMatrix;
}

bool D3DClass::CreateDeviceNContext()
{
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG) //����׸���϶� ����� ���� Ȱ��ȭ
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel; //D3DƯ¡ ����
	HRESULT hr = D3D11CreateDevice(
		0,													//�⺻ ���
		D3D_DRIVER_TYPE_HARDWARE,
		0,													//����Ʈ���� ��ġ�� ������� ����
		createDeviceFlags,
		0, 0,												//�⺻ ��� ������ ����
		D3D11_SDK_VERSION,
		&_Device,
		&featureLevel,
		&_DeviceContext
	);

	if(FAILED(hr))
	{
		MessageBox(NULL, _T("D3D11CreateDevice Failed!!"), NULL, MB_OK);
		return false;
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0) 
	{
		MessageBox(NULL, _T("Direct3D Feature Level 11unsupported"), NULL, MB_OK);
		return false;
	}

	return true;
}

bool D3DClass::CheckQualityLevels(UINT* m4xMsaaQuality)
{
	HRESULT hr = _Device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM,4, m4xMsaaQuality);
	if (FAILED(hr)) 
	{
		MessageBox(NULL, _T("4XMSAA Failed"), NULL, MB_OK);
		return false;
	}
	return true;
}

void D3DClass::FillSwapChainSt(int screenW, int screenH, HWND hWnd, bool IsFullScreen, UINT m4xMsaaQuality, DXGI_SWAP_CHAIN_DESC* swapChainDesc)
{
	// ������� ���̿� ���̸� ����
	swapChainDesc->BufferDesc.Width = screenW;
	swapChainDesc->BufferDesc.Height = screenH;
	//������� ���ΰ�ħ ������ ����
	swapChainDesc->BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc->BufferDesc.RefreshRate.Denominator = 1;
	//����۷� �Ϲ����� 32bit�� �����̽��� ����
	swapChainDesc->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//��ĵ������ ���İ� ��ĵ���̴��� �������� ����(unspecified)���� ����
	swapChainDesc->BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc->BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (_mEnable4xMsaa)
	{
		swapChainDesc->SampleDesc.Count = 4;
		swapChainDesc->SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else 
	{
		//��Ƽ ���ø� ����
		swapChainDesc->SampleDesc.Count = 1;
		swapChainDesc->SampleDesc.Quality = 0;
	}
	//������� �뵵�� ����
	swapChainDesc ->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//�ϳ��� ����۸��� ����ϵ��� ��
	swapChainDesc->BufferCount = 1;
	//�������� �̷���� WindowHandle����
	swapChainDesc->OutputWindow = hWnd;
	//window��� �Ǵ� FullScreen��带 ����
	if (IsFullScreen)
		swapChainDesc->Windowed = false;
	else
		swapChainDesc->Windowed = true;

	// ��µ� ������ ������� ������ ����
	swapChainDesc->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//�߰� �ɼ� �÷��׸� ������� ����
	swapChainDesc->Flags = 0;

}

bool D3DClass::CreateSwapChain(DXGI_SWAP_CHAIN_DESC *swapChainDesc)
{
	IDXGIDevice* dxgiDevice = NULL;
	HRESULT hr = _Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (FAILED(hr)) {
		MessageBox(NULL, _T("IDXGIDevice Failed"), NULL, MB_OK);
		return false;
	}

	IDXGIAdapter* dxgiAdapter = NULL;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	if (FAILED(hr)) {
		MessageBox(NULL, _T("IDXGIAdapte Failed"), NULL, MB_OK);
		return false;
	}

	//IDXGIFactoru �������̽��� ����
	IDXGIFactory* dxgiFactory = NULL;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if (FAILED(hr)) {
		MessageBox(NULL, _T("IDXGIFactory Failed"), NULL, MB_OK);
		return false;
	}

	//��ȯ �罽�� ����
	hr = dxgiFactory->CreateSwapChain(_Device, swapChainDesc, &_SwapChain);
	if (FAILED(hr)) {
		MessageBox(NULL, _T("IDXGISwapChain Failed"), NULL, MB_OK);
		return false;
	}

	//ȹ���� COM�������̽��� ����
	dxgiDevice->Release();
	dxgiDevice = NULL;
	dxgiAdapter->Release();
	dxgiAdapter = NULL;
	dxgiFactory->Release();
	dxgiFactory = NULL;

	return true;
}

void D3DClass::CreateRenderTargetV()
{
	ID3D11Texture2D* backBuffer = NULL;
	_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	_Device->CreateRenderTargetView(backBuffer, 0, &_RenderTargetView);
	backBuffer->Release();
	backBuffer = NULL;
}

bool D3DClass::CreatedepthNStencil(int screenW, int screenH, UINT m4xMsaaQuality)
{
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	//���� ������ description�� �ʱ�ȭ
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenW;
	depthBufferDesc.Height = screenH;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (_mEnable4xMsaa) 
	{
		depthBufferDesc.SampleDesc.Count = 4;
		depthBufferDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
	}
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//description�� ����Ͽ� ���� ������ �ؽ��ĸ� ����
	HRESULT hr = _Device->CreateTexture2D(&depthBufferDesc, NULL, &_DepthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, _T("_DepthStencilBuffer Failed"), NULL, MB_OK);
		return false;
	}
	//Stencil������ description�� �ʱ�ȭ
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	//Stencil������ description�� �ۼ�
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	//�ȼ��� ������ ���� stencil �۾�
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//�ȼ��� �������� ��� stencil�۾�
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Depth-StencilStatus ����
	hr = _Device->CreateDepthStencilState(&depthStencilDesc, &_DepthStencilState);
	if (FAILED(hr))
	{
		MessageBox(NULL, _T("_DepthStencilState Failed"), NULL, MB_OK);
		return false;
	}

	//Depth-StencilStatus ���� ����
	_DeviceContext->OMSetDepthStencilState(_DepthStencilState, 1);

	//Depth-StencilView�� description �ʱ�ȭ
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//Depth-StencilView�� Description�ۼ�
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = _Device->CreateDepthStencilView(_DepthStencilBuffer, &depthStencilViewDesc, &_DepthStencilView);
	if (FAILED(hr))
	{
		MessageBox(NULL, _T("_DepthStencilView Failed"), NULL, MB_OK);
		return false;
	}

	return true;
}

bool D3DClass::PiplineBindeing()
{
	_DeviceContext->OMSetRenderTargets(1, &_RenderTargetView, _DepthStencilView);

	D3D11_RASTERIZER_DESC rasterDesc;
	//��� ��� �׸����� ���� Rasterizer description �ۼ�
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//�ۼ��� description���κ��� Rasterizer ���� ����
	HRESULT hr = _Device->CreateRasterizerState(&rasterDesc, &_RasterState);
	if (FAILED(hr))
	{
		return false;
	}

	_DeviceContext->RSSetState(_RasterState);

	return true;
}

void D3DClass::SettingViewPort(int screenW, int screenH, float sDepth, float sNear)
{
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;
	viewport.Width = (float)screenW;
	viewport.Height = (float)screenH;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//Create ViewPort
	_DeviceContext->RSSetViewports(1, &viewport);

	//���� ��� ����
	fieldOfView = (float)XM_PI / 4.0f;
	screenAspect = (float)screenW / (float)screenH;
	
	//3D �������� ���� ��������� ����
	//(D3DMatrixPerspectiveFovLH)
	_ProjectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, sNear, sDepth);

	//���� ����� ���� ��ķ� �ʱ�ȭ. (D3DXMatrixIdentity)
	_WorldMatrix = XMMatrixIdentity();

	//2D�������� ���� ���翵 ����� ����
	_OrthoMatrix = XMMatrixOrthographicLH((float)screenW, (float)screenH, sDepth, sNear);
}
