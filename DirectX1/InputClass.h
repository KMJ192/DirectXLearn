#pragma once

class InputClass
{
private:
	bool _Keys[MAX_KEYS];

public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);
};

