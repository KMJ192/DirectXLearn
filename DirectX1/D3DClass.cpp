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

	//SwapChain description을 초기화
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	_Vsync_enabled = vSync;

	//1. D3D11CreateDevice함수를 이용하여 ID3D11Device 인터페이스와 장치 문맥, ID3D11DeviceContext 인터페이스 생성
	if (!CreateDeviceNContext())
		return false;

	//2. ID3D11Device::CheckMultisampleQualityLevels함수를 이용하여 4XMSAA품질 수준 지원 여부 점검
	if (!CheckQualityLevels(&m4xMsaaQuality))
		return false;

	//3. 생성할 교환 사슬의 특성을 서술하는 DXGI_SWAP_CHAIN_DESC구조체를 체움
	FillSwapChainSt(screenW, screenH, hWnd, isFullScreen, m4xMsaaQuality, &swapChainDesc);

	//4. 장치를 생성하는데 사용했던 IDXGIFactory 인터페이스를 질의해서 IDXGISwapChain인스턴스를 생성한다
	if (!CreateSwapChain(&swapChainDesc))
		return false;

	//5. 교환 사슬의 후면 버퍼에 대한 렌더 대상 뷰를 생성
	CreateRenderTargetV();
	
	//6. 깊이 스텐실 버퍼와 그에 연결되는 깊이, 스탠실 뷰를 생성
	if (!CreatedepthNStencil(screenW, screenH, m4xMsaaQuality))
		return false;

	//7. 렌더 대상 뷰와 깊이, 스탠실 뷰를 DirectX3D가사용할 수 있도록 렌더링 파이프라인의 출력 병합기 단계에 묶는다
	if (!PiplineBindeing())
		return false;

	//8. 뷰포트를 설정한다
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
	//버퍼를 어떤 색생으로 지울것인지 설정
	float color[4] = {red, green, blue, alpha};

	//백버퍼의 내용을 지움
	_DeviceContext->ClearRenderTargetView(_RenderTargetView, color);

	//깊이 버퍼를 지움
	_DeviceContext->ClearDepthStencilView(_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
	//렌더링이 완로되었으므로 백버퍼의 내용을 화면에 표시
	if (_Vsync_enabled) {
		//새로고침 비율을 고정
		_SwapChain->Present(1, 0);
	}
	else {
		//가능한 한 빠르게 표시
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
#if defined(DEBUG) || defined(_DEBUG) //디버그모드일때 디버그 계층 활성화
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel; //D3D특징 레벨
	HRESULT hr = D3D11CreateDevice(
		0,													//기본 어뎁터
		D3D_DRIVER_TYPE_HARDWARE,
		0,													//소프트웨어 장치를 사용하지 않음
		createDeviceFlags,
		0, 0,												//기본 기능 수준의 레벨
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
	// 백버퍼의 높이와 넓이를 설정
	swapChainDesc->BufferDesc.Width = screenW;
	swapChainDesc->BufferDesc.Height = screenH;
	//백버퍼의 새로고침 비율을 설정
	swapChainDesc->BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc->BufferDesc.RefreshRate.Denominator = 1;
	//백버퍼로 일반적인 32bit의 서페이스를 지정
	swapChainDesc->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//스캔라인의 정렬과 스캔라이닝을 지정되지 않음(unspecified)으로 설정
	swapChainDesc->BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc->BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (_mEnable4xMsaa)
	{
		swapChainDesc->SampleDesc.Count = 4;
		swapChainDesc->SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else 
	{
		//멀티 샘플링 종료
		swapChainDesc->SampleDesc.Count = 1;
		swapChainDesc->SampleDesc.Quality = 0;
	}
	//백버퍼의 용도를 설정
	swapChainDesc ->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//하나의 백버퍼만을 사용하도록 함
	swapChainDesc->BufferCount = 1;
	//렌더링이 이루어질 WindowHandle설정
	swapChainDesc->OutputWindow = hWnd;
	//window모드 또는 FullScreen모드를 설정
	if (IsFullScreen)
		swapChainDesc->Windowed = false;
	else
		swapChainDesc->Windowed = true;

	// 출력된 이후의 백버퍼의 내용을 버림
	swapChainDesc->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//추가 옵션 플래그를 사용하지 않음
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

	//IDXGIFactoru 인터페이스를 얻음
	IDXGIFactory* dxgiFactory = NULL;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if (FAILED(hr)) {
		MessageBox(NULL, _T("IDXGIFactory Failed"), NULL, MB_OK);
		return false;
	}

	//교환 사슬을 구성
	hr = dxgiFactory->CreateSwapChain(_Device, swapChainDesc, &_SwapChain);
	if (FAILED(hr)) {
		MessageBox(NULL, _T("IDXGISwapChain Failed"), NULL, MB_OK);
		return false;
	}

	//획득한 COM인터페이스를 해제
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

	//깊이 버퍼의 description을 초기화
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

	//description을 사용하여 깊이 버퍼의 텍스쳐를 생성
	HRESULT hr = _Device->CreateTexture2D(&depthBufferDesc, NULL, &_DepthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, _T("_DepthStencilBuffer Failed"), NULL, MB_OK);
		return false;
	}
	//Stencil상태의 description을 초기화
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	//Stencil상태의 description을 작성
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	//픽셀이 전면을 향한 stencil 작업
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//픽셀이 역방향인 경우 stencil작업
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Depth-StencilStatus 생성
	hr = _Device->CreateDepthStencilState(&depthStencilDesc, &_DepthStencilState);
	if (FAILED(hr))
	{
		MessageBox(NULL, _T("_DepthStencilState Failed"), NULL, MB_OK);
		return false;
	}

	//Depth-StencilStatus 상태 설정
	_DeviceContext->OMSetDepthStencilState(_DepthStencilState, 1);

	//Depth-StencilView의 description 초기화
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//Depth-StencilView의 Description작성
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
	//어떤걸 어떻게 그릴지에 대한 Rasterizer description 작성
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

	//작성한 description으로부터 Rasterizer 상태 생성
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

	//투영 행렬 설정
	fieldOfView = (float)XM_PI / 4.0f;
	screenAspect = (float)screenW / (float)screenH;
	
	//3D 렌더링을 위한 투영행렬을 생성
	//(D3DMatrixPerspectiveFovLH)
	_ProjectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, sNear, sDepth);

	//월드 행렬을 단위 행렬로 초기화. (D3DXMatrixIdentity)
	_WorldMatrix = XMMatrixIdentity();

	//2D렌더링에 사용될 정사영 행렬을 생성
	_OrthoMatrix = XMMatrixOrthographicLH((float)screenW, (float)screenH, sDepth, sNear);
}
