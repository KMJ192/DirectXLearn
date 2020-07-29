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

	//���콺 Ŀ���� ��ġ ������ ���� ȭ�� ũ�� ����
	_ScreenW = screenW;
	_ScreenH = screenH;

	//ȭ�鿡�� ���콺�� ��ġ�� �ʱ�ȭ
	_MouseX = 0;
	_MouseY = 0;

	//�⺻ ���� �Է� �������̽� �ʱ�ȭ
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_DirectInput, NULL);

	if (FAILED(result))
		return false;

	//Ű������ ���� �Է� �������̽��� �ʱ�ȭ
	result = _DirectInput->CreateDevice(GUID_SysKeyboard, &_Keyboard, NULL);
	if (FAILED(result))
		return false;

	//������ ������ ����, �̸� ���ǵ� Data ���� ��� ����(Ű����)
	result = _Keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;
	
	//�ٸ� ���α׷��� �������� �ʵ��� Ű���� ���� ���� ����
	result = _Keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
		return false;

	//Ű���带 ��������
	result = _Keyboard->Acquire();
	if (FAILED(result))
		return false;
	
	//���콺�� ���� �Է� �������̽��� �ʱ�ȭ
	result = _DirectInput->CreateDevice(GUID_SysMouse, &_Mouse, NULL);
	if (FAILED(result))
		return false;

	//�̸� ���ǵ� ���콺 ������ ������ ����Ͽ� ���콺 ������ ���� ����
	result = _Mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
		return false;

	//�ٸ� ���α׷��� �����ϵ��� ���콺�� ���� ���� ����
	result = _Mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	//���콺 ��������
	result = _Mouse->Acquire();
	if (FAILED(result))
		return false;

	return true;
}

void DInputClass::Release()
{
	//���콺 ����
	if (_Mouse)
	{
		_Mouse->Unacquire();
		_Mouse->Release();
		_Mouse = NULL;
	}

	//Ű���� ����
	if (_Keyboard)
	{
		_Keyboard->Unacquire();
		_Keyboard->Release();
		_Keyboard = NULL;
	}

	//DirectX ����
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

	//Ű������ ���� ���� �б�
	result = ReadKeyboard();
	if (!result)
		return false;

	//���콺�� ���� ���� �б�
	result = ReadMouse();
	if (!result)
		return false;

	//���콺 �� Ű������ ���� ���� ó��
	ProcessInput();

	return false;
}

bool DInputClass::ReadKeyboard()
{
	HRESULT result;
	
	//Ű���� ��ġ �б�
	result = _Keyboard->GetDeviceState(sizeof(_KeyboardState), (LPVOID)&_KeyboardState);
	if (FAILED(result))
	{
		//Ű���尡 ��Ŀ���� �о��ų� ȹ����� ���� ��� ��Ʈ���� �ٽ� �������� ��
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
		//���콺�� ��Ŀ���� �о��ų� ȹ������ ���� ��� ��Ʈ���� �ٽ� �������� ��
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			_Mouse->Acquire();
		else
			return false;
	}

	return true;
}

void DInputClass::ProcessInput()
{
	//������ ���� ���콺 ��ġ�� ������ ������� ���콺 Ŀ���� ��ġ ������Ʈ
	_MouseX += _MouseState.lX;
	_MouseY += _MouseState.lY;

	//���콺 ��ġ�� ȭ�� �ʺ� �Ǵ� ���̸� �ʰ����� �ʴ��� Ȯ��
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
	//ex) ��Ʈ�� Ű���� ���·� ESCŰ�� ���� ������ �ִ��� Ȯ��
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


