#include "Stdafx.h"
#include "ApplicationClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	int nMsg = 0;
	if (ApplicationClass::GetInstance()->Initialize(hInstance, false, 1280, 720)) 
	{
		nMsg = ApplicationClass::GetInstance()->Run();
	}
	ApplicationClass::GetInstance()->DestoryInstance();

	return nMsg;
}