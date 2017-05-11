#ifndef Target_h__
#define Target_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTarget :public CBase
{
private:
	LPDIRECT3DDEVICE9		m_pDevice;
	LPDIRECT3DTEXTURE9		m_pTargetTexture;
	LPDIRECT3DSURFACE9		m_pTargetSurface;
	LPDIRECT3DSURFACE9		m_pOldSurface;

	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

private:
	D3DXCOLOR				m_Color;
	_bool					m_bExcept;


public:
	HRESULT Initialize(const _uint& iSizeX, const _uint& iSizeY, D3DFORMAT Format, D3DXCOLOR Color);
	HRESULT	Initialize_Debug(const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);

	void	Render_Debug(void);
	void	ClearTarget(void);
	void	SetUpDevice(const _uint& iIndex);
	void	ResetDevice(const _uint& iIndex);

	void	SetUpShader(LPD3DXEFFECT pEffect, const char* pTexKey);
	void	SetExcept(_bool bTF) { m_bExcept = bTF; }

	_bool	GetExcept(void) { return m_bExcept; }
public:
	virtual void Free(void);

public:
	static CTarget* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iSizeX, const _uint& iSizeY, D3DFORMAT Format, D3DXCOLOR Color);

private:
	explicit CTarget(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CTarget(void);
};

END
#endif // Target_h__