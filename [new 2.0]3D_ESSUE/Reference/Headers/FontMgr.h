#ifndef FontMgr_h__
#define FontMgr_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CFont;
class ENGINE_DLL CFontMgr :public CBase
{
private:
	DECLARE_SINGLETON(CFontMgr)

private:
	typedef	map<const TCHAR*, CFont*>		MAPFONT;
	MAPFONT									m_mapFont;

private:
	CFont*	FindFont(const _tchar* pFontKey);

public:
	HRESULT	AddFont(LPDIRECT3DDEVICE9 pDevice, const _tchar* pFontKey, const _tchar* pFontName, const UINT& iHeight, const _uint& iWidth, const _uint& iWeight);
	void	Render(const _tchar* pFontKey, const _tchar* pString, const MyVec3& vPos, D3DXCOLOR Color);

public:
	virtual void Free(void);

private:
	explicit CFontMgr(void);
	virtual ~CFontMgr(void);

};

END
#endif // FontMgr_h__
