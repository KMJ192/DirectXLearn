#pragma once

#include <d3d11.h>
#include <fstream>

using namespace std;

void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hWnd, const TCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	//���� �޽����� ��� �մ� ���ڿ� ������ �����͸� ������
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//�޽����� ���̸� ������
	bufferSize = errorMessage->GetBufferSize();

	//������ ���� log ���
	fout.open("shader-error.txt");

	//���� �޽��� ���
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}
	//������ ����
	fout.close();

	//���� �޽��� ��ȯ
	errorMessage->Release();
	errorMessage = 0;

	//������ ������ ������ �˾� �޽����� �˷���
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