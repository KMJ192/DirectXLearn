#include "Stdafx.h"

#include "D3DUtilClass.h"
#include "ModelFileInfoClass.h"
#include "ObjectEnumHelper.h"

void ExtractName(TCHAR* pOut, char pIn);

ModelFileInfoClass::ModelFileInfoClass()
{
	_FileInfo = NULL;
}

ModelFileInfoClass::ModelFileInfoClass(const ModelFileInfoClass& other)
{
}

ModelFileInfoClass::~ModelFileInfoClass()
{
}

bool ModelFileInfoClass::InitializeInfo(TCHAR* filename)
{
	if (!LoadFile(filename))
		return false;

	return true;
}

void ModelFileInfoClass::ReleaseInfo()
{
	if (_FileInfo != NULL) 
	{
		delete _FileInfo;
		_FileInfo = NULL;
	}
}

bool ModelFileInfoClass::LoadFile(TCHAR* filename)
{
	if (_FileInfo == NULL)
	{
		_FileInfo = new CharacterFileInfo;
	}

	ifstream fin;
	char input[MAX_PATH] = { 0, };
	char buff[MAX_PATH] = { 0, };
	char data[MAX_PATH] = { 0, };

	fin.open(filename);

	if (fin.fail())
		return false;

	while (!fin.getline(input, sizeof(input)).eof())
	{
		sscanf_s(input, "%s", buff, MAX_PATH);

		if (!_stricmp(buff, "FILENAME"))
		{
			sscanf_s(input, "%s %s", buff, MAX_PATH, data, MAX_PATH);
			ExtractName(_FileInfo->szFileName, data);
		}

		if (!_stricmp(buff, "SIZE_U"))
		{
			sscanf_s(input, "%s %f", buff, MAX_PATH, &_FileInfo->fsizeU);
		}

		if (!_stricmp(buff, "SIZE_V"))
		{
			sscanf_s(input, "%s %f", buff, MAX_PATH, &_FileInfo->fsizeV);
		}

		if (!_stricmp(buff, "ANISTATE_CNT_1"))
		{
			sscanf_s(input, "%s %d", buff, MAX_PATH, &_FileInfo->aniMaxCnt[0]);
		}

		if (!_stricmp(buff, "ANISTATE_CNT_2"))
		{
			sscanf_s(input, "%s %d", buff, MAX_PATH, &_FileInfo->aniMaxCnt[1]);
		}

		if (!_stricmp(buff, "ANISTATE_CNT_3"))
		{
			sscanf_s(input, "%s %d", buff, MAX_PATH, &_FileInfo->aniMaxCnt[2]);
		}

		if (!_stricmp(buff, "ANISTATE_CNT_4"))
		{
			sscanf_s(input, "%s %d", buff, MAX_PATH, &_FileInfo->aniMaxCnt[3]);
		}

		if (!_stricmp(buff, "ANISTATE_CNT_5"))
		{
			sscanf_s(input, "%s %d", buff, MAX_PATH, &_FileInfo->aniMaxCnt[4]);
		}

		if (!_stricmp(buff, "ANISTATE_CNT_6"))
		{
			sscanf_s(input, "%s %d", buff, MAX_PATH, &_FileInfo->aniMaxCnt[5]);
		}
		if (!_stricmp(buff, "ANISTATE_CNT_7"))
		{
			sscanf_s(input, "%s %d", buff, MAX_PATH, &_FileInfo->aniMaxCnt[6]);
		}
	}
	//modelInfo ÆÄÀÏÀ» close
	fin.close();

	return true;
}
