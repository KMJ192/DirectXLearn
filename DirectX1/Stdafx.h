#pragma once

#define MAX_KEYS 256
#define WIN32_LEAN_AND_MEAN

#include<stdio.h>
#include<Windows.h>
#include<tchar.h>
#include <iostream>

#include<d3d11.h>
#include<d3dcommon.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<DirectXPackedVector.h>

#include "InputClass.h"
#include "GraphicsClass.h"

using namespace std;
using namespace DirectX;

template <typename T>
class Singleton
{
protected:
	Singleton() {}
	virtual ~Singleton() {}
public:
	static T* GetInstance()
	{
		if (_Instance == NULL) _Instance = new T;
		return _Instance;
	};

	static void DestoryInstance()
	{
		if (_Instance)
		{
			delete _Instance;
			_Instance = NULL;
		}
	}
private:
	static T* _Instance;
};

template <typename T> T* Singleton<T>::_Instance = NULL;