#pragma once

#include "ObjectEnumHelper.h"

class ModelFileInfoClass
{
	bool LoadFile(TCHAR*);

protected:
	CharacterFileInfo* _FileInfo;

public:
	ModelFileInfoClass();
	ModelFileInfoClass(const ModelFileInfoClass&);
	~ModelFileInfoClass();

	bool InitializeInfo(TCHAR*);
	void ReleaseInfo();
};

