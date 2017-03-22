#ifndef MainApp_h__
#define MainApp_h__

#include "BigHeaders.h"
#include "Base.h"
#include "Export_Engine.h"

class CMainApp :public CBase
{
private:
	LPDIRECT3DDEVICE9	m_pDevice;

private:
	_tchar		m_szFPS[128];
	_ulong		m_dwCnt;
	_float		m_fTimeAcc;

private:
	HRESULT	Ready_Setting(void);
	HRESULT	Ready_Component(void);

public:
	HRESULT	Initialize(void);
	_int	Update(const _float& fDelta);
	void	Render(void);

private:
	virtual void	Free(void);

public:
	static CMainApp* Create(void);

private:
	explicit CMainApp(void);
	virtual ~CMainApp(void);
};

#endif // MainApp_h__