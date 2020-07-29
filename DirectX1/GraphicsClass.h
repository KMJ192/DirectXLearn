#pragma once

class D3DClass;
class Camera;
class SpriteClass;
class ModelTriangle;
class ModelRectangle;
class ColorShader;
class TextureShader;

class GraphicsClass
{
	bool _Vsync_enabled;
	float _SDepth;
	float _SNear;

private:
	D3DClass*_D3DC;
	Camera* _Camera;
	SpriteClass* _Sprite;
	ModelRectangle* _ModelRectangle;
	ModelTriangle* _ModelTriangle;
	ColorShader* _ColorShader;
	TextureShader* _TextureShader;

public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND, bool);
	void Release();
	bool Frame();

private:
	bool Render();	
};

