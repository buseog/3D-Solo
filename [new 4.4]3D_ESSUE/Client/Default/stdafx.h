// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C�� ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <d3dx9.h>
#include <d3d9.h>

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <process.h>

//����
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