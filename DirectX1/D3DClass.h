#pragma once

class D3DClass
{
private:
	bool _Vsync_enabled;
	ID3D11Device* _Device;
	ID3D11DeviceContext* _DeviceContext;
	IDXGISwapChain* _SwapChain;
	ID3D11RenderTargetView* _RenderTargetView;
	ID3D11Texture2D* _DepthStencilBuffer;
	ID3D11DepthStencilState* _DepthStencilState;
	ID3D11DepthStencilState* _DepthDisabledStencilState;
	ID3D11DepthStencilView* _DepthStencilView;
	ID3D11RasterizerState* _RasterState;

	bool _mEnable4xMsaa;
	XMMATRIX _ProjectionMatrix;
	XMMATRIX _WorldMatrix;
	XMMATRIX _OrthoMatrix;

	ID3D11BlendState* _AlphaEnableBlendingState;
	ID3D11BlendState* _AlphaDisableBlendingState;

public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Release();
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void TurnZBufferOn();
	void TurnZBufferOff();
	void EnableAlphaBlending();
	void DisableAlphaBlending();

private:
	bool CreateDeviceNContext();
	bool CheckQualityLevels(UINT*);
	void FillSwapChainSt(int, int, HWND, bool, UINT, DXGI_SWAP_CHAIN_DESC*);
	bool CreateSwapChain(DXGI_SWAP_CHAIN_DESC*);
	void CreateRenderTargetV();
	bool CreatedepthNStencil(int, int, UINT);
	bool PiplineBindeing();
	void SettingViewPort(int, int, float, float);
};
