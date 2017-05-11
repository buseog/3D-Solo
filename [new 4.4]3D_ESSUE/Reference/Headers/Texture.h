#ifndef Texture_h__
#define Texture_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture :public CComponent
{
public:
	enum TEXTURETYPE { TEX_NORMAL, TEX_CUBE, TEX_END };
private:
	LPDIRECT3DDEVICE9	m_pDevice;

private:
	typedef vector<IDirect3DBaseTexture9*>	VECTEXUTRE;
	VECTEXUTRE		m_vecTexture;

public:
	void SetUpShader(LPD3DXEFFECT pEffect, const _uint& iIndex = 0);
	void SetUpShader(LPD3DXEFFECT pEffect, const char* pTexKey, const _uint& iIndex = 0);

public:
	virtual HRESULT Initialize(const _tchar* pPath, TEXTURETYPE eType, const _uint& iCnt);
	virtual HRESULT Initialize(const _tchar* pPath, TEXTURETYPE eType, const _uint& iCnt, const _uint* pIndex);
	virtual void	Render(const _uint& iIndex = 0);

public:
	virtual void	Free(void);
	virtual CComponent* Clone(void);

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pPath, TEXTURETYPE eType, const _uint& iCnt = 1);
	static CTexture* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pPath, TEXTURETYPE eType, const _uint* pIndex, const _uint& iCnt = 1);

protected:
	explicit CTexture(LPDIRECT3DDEVICE9 pDevice);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture(void);
};

END
#endif // Texture_h__