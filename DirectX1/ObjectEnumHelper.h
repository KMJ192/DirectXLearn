#pragma once

enum PlayerAnimationType
{
	ANI_MOVE = 0,
	ANI_SPELL,
	ANI_DEAD,
	ANI_CAST,
	ANI_IDLE,
	ANI_VICTORY,
	ANI_VIEW,

	MAX_ANI_CNT,

	ANI_U_INDEX_MAX = 6
};

// structs
typedef struct CharacterFileInfo
{
	TCHAR szFileName[MAX_PATH];
	float fsizeU;
	float fsizeV;
	int aniMaxCnt[MAX_ANI_CNT];

	CharacterFileInfo()
	{
		ZeroMemory(szFileName, sizeof(szFileName));
		fsizeU = fsizeV = 0.0f;
		memset(aniMaxCnt, 0, sizeof(aniMaxCnt));
	}
}_CHARACTERFILEINFO, * _LPCHARACTERFILEINFO;