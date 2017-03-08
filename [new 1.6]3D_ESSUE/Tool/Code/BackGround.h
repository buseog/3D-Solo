#ifndef BackGround_h__
#define BackGround_h__

#include "../Default/stdafx.h"

class CBackGround
{
private:
	LPDIRECT3DDEVICE9 m_pDevice;

public:
	HRESULT Initialize(void);

public:
	static CBackGround* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CBackGround(void);
};

#endif // BackGround_h__