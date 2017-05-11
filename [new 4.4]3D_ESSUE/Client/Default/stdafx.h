// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <d3dx9.h>
#include <d3d9.h>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <process.h>

//사운드
#include <iostream>
#include <io.h>

#include "fmod.h"
#include "fmod.hpp"
#include "fmod_dsp.h"
#include "fmod_errors.h"

#pragma comment(lib, "fmodex_vc.lib")

class CStringCompare
{
public:
	explicit CStringCompare(const TCHAR* pKey)
		:m_pString(pKey){}
	~CStringCompare(){}
public:
	template<typename T>
	bool operator () (T Data)
	{
		return !lstrcmp(Data.first, m_pString);
	}
private:
	const TCHAR*	m_pString;
};