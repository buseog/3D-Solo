#pragma once

extern HWND g_hWnd;

const int WINCX = 1024;
const int WINCY = 768;

typedef signed char			_byte;
typedef unsigned char		_ubyte;

typedef wchar_t				_tchar;

typedef signed int			_int;
typedef unsigned int		_uint;

typedef signed short		_short;
typedef unsigned short		_ushort;

typedef signed long			_long;
typedef unsigned long		_ulong;

typedef float				_float;

typedef double				_double;

typedef bool				_bool;

typedef D3DXVECTOR2			MyVec2;

typedef struct tagMyVector3 :public D3DXVECTOR3
{
	tagMyVector3(void)	{}

	tagMyVector3(const _float& fX, const _float& fY, const _float& fZ)
		:D3DXVECTOR3(fX, fY, fZ)	{}

	tagMyVector3(const tagMyVector3& rhs)
		:D3DXVECTOR3(rhs)	{}

	tagMyVector3(const D3DXVECTOR3& rhs)
		:D3DXVECTOR3(rhs)	{}

	~tagMyVector3(void)	{}

	void operator = (const tagMyVector3& Vector)
	{
		x = Vector.x;
		y = Vector.y;
		z = Vector.z;
	}

	void operator = (const D3DXVECTOR3& Vector)
	{
		x = Vector.x;
		y = Vector.y;
		z = Vector.z;
	}

	float Get_Lenth(void)
	{
		return sqrtf(x * x + y * y + z * z);
	}

	D3DXVECTOR3 Get_Normalize(void)
	{
		float fDis = sqrtf(x * x + y * y + z * z);

		return D3DXVECTOR3(x / fDis, y / fDis, z / fDis);;
	}

	float Get_Distance(const D3DXVECTOR3* pDest)
	{
		D3DXVECTOR3 vTemp = *pDest - D3DXVECTOR3(x, y, z);

		return sqrtf(vTemp.x * vTemp.x + vTemp.y * vTemp.y + vTemp.z * vTemp.z);
	}
}MyVec3;

typedef D3DXVECTOR4			MyVec4;

typedef D3DXMATRIX			MyMat;

enum SCENEID { SCENE_STATIC, SCENE_END };