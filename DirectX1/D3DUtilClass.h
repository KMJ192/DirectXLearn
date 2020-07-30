#pragma once

#include <d3d11.h>
#include <fstream>

using namespace std;

void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hWnd, const TCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	//에러 메시지를 담고 잇는 문자열 버퍼의 포인터를 가져옴
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//메시지의 길이를 가져옴
	bufferSize = errorMessage->GetBufferSize();

	//파일을 열고 log 기록
	fout.open("shader-error.txt");

	//에러 메시지 기록
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}
	//파일을 닫음
	fout.close();

	//에러 메시지 반환
	errorMessage->Release();
	errorMessage = 0;

	//컴파일 에러가 있음을 팝업 메시지로 알려줌
	MessageBox(hWnd, _T("Error compiling shader. Check shader-error.txt for message"), shaderFilename, MB_OK);

	return;

}

void ExtractName(TCHAR* pOut, char* pIn)
{
	char szTemp[MAX_PATH] = { 0, };

	int iLength = strlen(pIn);
	int i, j;
	for (i = 0; i < iLength; ++i)
	{
		if (pIn[i] == '"')
		{
			for (j = ++i; j < iLength; ++j)
			{
				if (pIn[j] == '"')		break;

				szTemp[j - i] = pIn[j];
			}
			szTemp[j - i] = '\0';
			break;
		}
	}

	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)szTemp, -1, pOut, iLength);
}