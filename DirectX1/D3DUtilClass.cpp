#include "Stdafx.h"
#include <fstream>
#include "D3DUtilClass.h"

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