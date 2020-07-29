#include "Stdafx.h"
#include "DInputClass.h"

DInputClass::DInputClass()
{
	_DirectInput = NULL;
	_Keyboard = NULL;
	_Mouse = NULL;
}

DInputClass::~DInputClass()
{
}

bool DInputClass::Initialize(HINSTANCE hInstance, HWND hWnd, int screenW, int screenH)
{
	HRESULT result;

	//마우스 커서의 위치 지정에 사용될 화면 크기 저장
	_ScreenW = screenW;
	_ScreenH = screenH;

	//화면에서 마우스의 위치를 초기화
	_MouseX = 0;
	_MouseY = 0;

	//기본 직접 입력 인터페이스 초기화
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_DirectInput, NULL);

	if (FAILED(result))
		return false;

	//키보드의 직접 입력 인터페이스를 초기화
	result = _DirectInput->CreateDevice(GUID_SysKeyboard, &_Keyboard, NULL);
	if (FAILED(result))
		return false;

	//데이터 형식을 설정, 미리 정의된 Data 형식 사용 가능(키보드)
	result = _Keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;
	
	//다른 프로그램과 공유하지 않도록 키보드 협조 수준 설정
	result = _Keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
		return false;

	//키보드를 가져오기
	result = _Keyboard->Acquire();
	if (FAILED(result))
		return false;
	
	//마우스의 직접 입력 인터페이스를 초기화
	result = _DirectInput->CreateDevice(GUID_SysMouse, &_Mouse, NULL);
	if (FAILED(result))
		return false;

	//미리 정의된 마우스 데이터 형식을 사용하여 마우스 데이터 형식 설정
	result = _Mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
		return false;

	//다른 프로그램과 공유하도록 마우스의 협력 수준 설정
	result = _Mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	//마우스 가져오기
	result = _Mouse->Acquire();
	if (FAILED(result))
		return false;

	return true;
}

void DInputClass::Release()
{
	//마우스 해제
	if (_Mouse)
	{
		_Mouse->Unacquire();
		_Mouse->Release();
		_Mouse = NULL;
	}

	//키보드 해제
	if (_Keyboard)
	{
		_Keyboard->Unacquire();
		_Keyboard->Release();
		_Keyboard = NULL;
	}

	//DirectX 해제
	if (_DirectInput)
	{
		_DirectInput->Release();
		_DirectInput = NULL;
	}

	return;
}

bool DInputClass::Frame()
{
	bool result;

	//키보드의 현재 상태 읽기
	result = ReadKeyboard();
	if (!result)
		return false;

	//마우스의 현재 상태 읽기
	result = ReadMouse();
	if (!result)
		return false;

	//마우스 및 키보드의 변경 사항 처리
	ProcessInput();

	return false;
}

bool DInputClass::ReadKeyboard()
{
	HRESULT result;
	
	//키보드 장치 읽기
	result = _Keyboard->GetDeviceState(sizeof(_KeyboardState), (LPVOID)&_KeyboardState);
	if (FAILED(result))
	{
		//키보드가 포커스를 읽었거나 획득되지 않은 경우 컨트롤을 다시 얻으려고 함
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			_Keyboard->Acquire();
		else
			return false;
	}

	return false;
}

bool DInputClass::ReadMouse()
{
	HRESULT result;
	result = _Mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&_MouseState);
	if (FAILED(result))
	{
		//마우스가 포커스를 읽었거나 획득죄이 낳은 경우 컨트롤을 다시 얻으려고 함
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			_Mouse->Acquire();
		else
			return false;
	}

	return true;
}

void DInputClass::ProcessInput()
{
	//프레임 동안 마우스 위치의 변경을 기반으로 마우스 커서의 위치 업데이트
	_MouseX += _MouseState.lX;
	_MouseY += _MouseState.lY;

	//마우스 위치가 화면 너비 또는 높이를 초과하지 않는지 확인
	if (_MouseX < 0)
		_MouseX = 0;

	if (_MouseY < 0)
		_MouseY = 0;
	
	if (_MouseX > _ScreenW)
		_MouseX = _ScreenW;

	if (_MouseY > _ScreenH)
		_MouseY = _ScreenH;

	return;
}

bool DInputClass::IsEscapePressed()
{
	//ex) 비트와 키보드 상태로 ESC키가 현재 눌러져 있는지 확인
	if (_KeyboardState[DIK_ESCAPE] & 0x80)
		return true;

	return false;
}

void DInputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = _MouseX;
	mouseY = _MouseY;

	return;
}


