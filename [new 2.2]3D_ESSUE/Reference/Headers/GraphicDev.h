#ifndef GraphicDev_h__
#define GraphicDev_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGraphicDev :public CBase
{
	DECLARE_SINGLETON(CGraphicDev)

public:
	enum WINMODE {MODE_FULL, MODE_WIN};

private:
	LPDIRECT3D9					m_pSDK;
	LPDIRECT3DDEVICE9			m_pDevice;

private:
	void SetParameters(D3DPRESENT_PARAMETERS& d3dpp, WINMODE Mode, HWND hWnd, const WORD& wSizeX, const WORD& wSizeY);

public:
	HRESULT InitGraphicDev(HWND hWnd, WINMODE Mode, const _uint& wSizeX, const _uint& wSizeY);
	void	Render_Begin(void);
	void	Render_End(void);

public:
	LPDIRECT3DDEVICE9 GetDevice(void);
	
public:
	virtual void Free(void);

private:
	explicit CGraphicDev(void);
	virtual ~CGraphicDev(void);
};

END

#endif // GraphicDev_h__
