#pragma once

class D3DClass;

class GraphicsClass
{
	bool _Vsync_enabled;
	float _SDepth;
	float _SNear;

	D3DClass *_D3DC;
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
	
};

