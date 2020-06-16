#include "Stdafx.h"
#include "ApplicationClass.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		//Window Delete
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		//Window Close
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}

	return ApplicationClass::GetInstance()->ApplicationProc(hWnd, uMsg, wParam, lParam);
}

ApplicationClass::ApplicationClass()
{
	_InputC = NULL;
	_GraphicsC = NULL;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(HINSTANCE hInst, bool isFullScreen, int screenW, int screenH)
{
	bool result = true;
	InitializeWindows(hInst, isFullScreen, screenW, screenH);

	//Input��ü ����
	if (_InputC == NULL) {
		_InputC = new InputClass;
		if (!_InputC) return false;
	}

	//Initialize Object
	_InputC->Initialize();

	if (_GraphicsC == NULL) {
		_GraphicsC = new GraphicsClass;
		if (!_GraphicsC) return false;
	}
	result = _GraphicsC->Initialize(screenW, screenH, _hWnd, isFullScreen);
	if (!result) {
		return false;
	}

	return result;
}

void ApplicationClass::Shutdown()
{
	//Object��ȯ
	if (_GraphicsC) {
		_GraphicsC->Release();
		delete _GraphicsC;
		_GraphicsC = NULL;
	}

	if (_InputC != NULL) {
		delete _InputC;
		_InputC = NULL;
	}
	ShutdownWindows();
}

int ApplicationClass::Run()
{
	bool done;
	ZeroMemory(&_Msg, sizeof(MSG));
	done = false;
	while (!done) {
		if (PeekMessage(&_Msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&_Msg);
			DispatchMessage(&_Msg);
		}
		if (_Msg.message == WM_QUIT) {
			//Window���� Application�� ���Ḧ ��û�ϴ� ��� ��������
			done = true;
		}
		else {
			//Frame function ����
			done = !Frame();
		}
	}
	Shutdown();
	return (int)_Msg.wParam;
}


bool ApplicationClass::Frame()
{
	//ESCŰ�� ����
	if (_InputC->IsKeyDown(VK_ESCAPE)) {
		return false;
	}

	//graphics Object�� �۾��� ó��
	if (!_GraphicsC->Frame()) {
		return false;
	}
	return true;
}

LRESULT ApplicationClass::ApplicationProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		//Ű�� ������ ���
		_InputC->KeyDown((unsigned int)wParam);
		break;
	case WM_KEYUP:
		//Ű�� �����Ȱ� input��ü�� ����
		_InputC->KeyUp((unsigned int)wParam);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void ApplicationClass::InitializeWindows(HINSTANCE hInst, bool isFullScreen, int& screenW, int& screenH)
{
	WNDCLASSEX wc;
	//DEVMODE �����ͱ����� ������ �Ǵ� ���÷��� ��ġ�� �ʱ�ȭ �� ȯ�濡 ���� ������ �����մϴ�
	DEVMODE dmScreenSettings;
	int posX, posY;

	//������ ��ũ�� ��� ����
	_IsFullScreen = isFullScreen;

	//Application�� �ν��Ͻ� Get
	_hInstance = hInst;

	//Application�� �̸� ����
	_ApplicationName = _T("FrameWork");

	//WindowŬ������ �⺻ �������� ����
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _ApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//window class ���
	RegisterClassEx(&wc);

	if (_IsFullScreen) {
		//ȭ���� �ػ� ��������
		screenW = GetSystemMetrics(SM_CXSCREEN);
		screenH = GetSystemMetrics(SM_CYSCREEN);

		//ȭ�� ũ�⸦ ����ũ�� ũ�⿡ ���߰� ������ 32bit�� ����
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenW;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenH;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Ǯ��ũ���� �´� ���÷��� ����
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		//�������� ��ġ�� ȭ���� ���� ���� ����
		posX = posY = 0;
	}
	else {
		//Location window position in center of screen 
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenW) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenH) / 2;
	}

	_hWnd = CreateWindowEx(WS_EX_APPWINDOW, _ApplicationName, _ApplicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
		posX, posY, screenW, screenH, NULL, NULL, _hInstance, NULL);

	ShowWindow(_hWnd, SW_SHOW);
	SetForegroundWindow(_hWnd);
	SetFocus(_hWnd);

	//Ŀ�� ǥ��
	ShowCursor(true);
}

void ApplicationClass::ShutdownWindows()
{
	ShowCursor(true);
	if (_IsFullScreen) {
		ChangeDisplaySettings(NULL, 0);
	}
	DestroyWindow(_hWnd);
	_hWnd = NULL;

	//Application Instance ����
	UnregisterClass(_ApplicationName, _hInstance);
	_hInstance = NULL;
}
