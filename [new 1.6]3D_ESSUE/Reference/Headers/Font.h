#ifndef Font_h__
#define Font_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CFont :public CBase
{
private:
	LPDIRECT3DDEVICE9		m_pDevice;
	LPD3DXFONT				m_pFont;
	UINT					m_iHeight;
	UINT					m_iWidth;

public:
	HRESULT	InitFont(const _tchar* pFaceName, const _uint& iHeight, const _uint& iWidth, const _uint& iWeight);
	void	Render(const _tchar* pString, const MyVec3& vPos, D3DXCOLOR Color);

public:
	static CFont* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pFaceName, const _uint& iHeight, const _uint& iWidth, const _uint& iWeight);

public:
	virtual void Free(void);

private:
	explicit CFont(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CFont(void);
};

END
#endif // Font_h__
