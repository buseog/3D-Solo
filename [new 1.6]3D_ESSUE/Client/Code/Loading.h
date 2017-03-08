#ifndef Loading_h__
#define Loading_h__

#include "BigHeaders.h"
#include "Base.h"

class CLoading :public CBase
{
public:
	enum LOADINGID { LOADING_STAGE, LOADING_BOSS, LOADING_END };

private:
	LPDIRECT3DDEVICE9	m_pDevice;

private:
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_Crt;
	LOADINGID			m_eID;

private:
	_tchar				m_szString[128];
	_bool				m_bComplete;

public:
	CRITICAL_SECTION*	GetCriticalSection(void);
	LOADINGID GetLoadingID(void);
	_bool	GetComplete(void);
	_tchar*	GetString(void);

public:
	virtual HRESULT	Initialize(LOADINGID eID);
	_uint	Loading_Stage(void);

public:
	static unsigned int APIENTRY ThreadMain(void* pArg);

public:
	virtual void Free(void);

public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pDevice, LOADINGID eID);

private:
	explicit CLoading(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CLoading(void);
};

#endif // Loading_h__