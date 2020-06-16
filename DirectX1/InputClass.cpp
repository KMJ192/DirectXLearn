#include"Stdafx.h"
#include "InputClass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass& other)
{
}
InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	for (int i = 0; i < MAX_KEYS; i++) {
		_Keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int key)
{
	_Keys[key] = true;
}

void InputClass::KeyUp(unsigned int key)
{
	_Keys[key] = false;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	return _Keys[key];
}