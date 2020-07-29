#pragma once

class DInputClass
{
public:
	DInputClass();
	~DInputClass();

	bool Initialize(HINSTANCE hInstance, HWND hWnd, int screenW, int screenH);
	void Release();
	bool Frame();
	bool IsEscapePressed();
	void GetMouseLocation(int& mouseX, int& mouseY);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	IDirectInput8* _DirectInput;
	IDirectInputDevice8* _Keyboard;
	IDirectInputDevice8* _Mouse;
	unsigned char _KeyboardState[256];
	DIMOUSESTATE _MouseState;
	int _ScreenW;
	int _ScreenH;
	int _MouseX;
	int _MouseY;

};

