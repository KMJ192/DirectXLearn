#pragma once

class D3DClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND, bool);
	void Release();
	bool Frame();

private:
	bool Render();

private:
	D3DClass* mDirect3D;
};

