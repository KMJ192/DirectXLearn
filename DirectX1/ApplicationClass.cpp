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

	//Input객체 생성
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
	//Object반환
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
			//Window에서 Application의 종료를 요청하는 경우 빠저나감
			done = true;
		}
		else {
			//Frame function 실행
			done = !Frame();
		}
	}
	Shutdown();
	return (int)_Msg.wParam;
}


bool ApplicationClass::Frame()
{
	//ESC키로 종료
	if (_InputC->IsKeyDown(VK_ESCAPE)) {
		return false;
	}

	//graphics Object의 작업을 처리
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
		//키가 눌린것 기록
		_InputC->KeyDown((unsigned int)wParam);
		break;
	case WM_KEYUP:
		//키가 해제된건 input객체에 전달
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
	//DEVMODE 데이터구조는 프린터 또는 디스플레이 장치의 초기화 및 환경에 대한 정보를 포함합니다
	DEVMODE dmScreenSettings;
	int posX, posY;

	//윈도우 스크린 모드 설정
	_IsFullScreen = isFullScreen;

	//Application의 인스턴스 Get
	_hInstance = hInst;

	//Application의 이름 설정
	_ApplicationName = _T("FrameWork");

	//Window클래스를 기본 설정으로 맞춤
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

	//window class 등록
	RegisterClassEx(&wc);

	if (_IsFullScreen) {
		//화면의 해상도 가져오기
		screenW = GetSystemMetrics(SM_CXSCREEN);
		screenH = GetSystemMetrics(SM_CYSCREEN);

		//화면 크기를 데스크톱 크기에 맞추고 색상을 32bit로 적용
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenW;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenH;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//풀스크린에 맞는 디스플레이 설정
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		//윈도우의 위치를 화면의 왼쪽 위로 맞춤
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

	//커서 표시
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

	//Application Instance 제거
	UnregisterClass(_ApplicationName, _hInstance);
	_hInstance = NULL;
}
