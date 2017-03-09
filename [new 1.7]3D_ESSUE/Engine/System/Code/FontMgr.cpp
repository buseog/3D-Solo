#include "FontMgr.h"
#include "Font.h"

USING(Engine)
IMPLEMENT_SINGLETON(CFontMgr)

Engine::CFontMgr::CFontMgr(void)
{

}

Engine::CFontMgr::~CFontMgr(void)
{
	
}

HRESULT Engine::CFontMgr::AddFont(LPDIRECT3DDEVICE9 pDevice, const _tchar* pFontKey, const _tchar* pFontName,const _uint& iHeight, const _uint& iWidth, const _uint& iWeight)
{
	CFont* pFont = FindFont(pFontKey);

	if (pFont != NULL)
		return E_FAIL;

	pFont = CFont::Create(pDevice, pFontKey, iHeight, iWidth, iWeight);
	
	if (pFont == NULL)
		return E_FAIL;

	m_mapFont.insert(MAPFONT::value_type(pFontKey, pFont));

	return S_OK;
}

void Engine::CFontMgr::Render(const TCHAR* pFontKey, const _tchar* pString, const MyVec3& vPos, D3DXCOLOR Color)
{
	CFont* pFont = FindFont(pFontKey);

	if (pFont == NULL)
		return;

	pFont->Render(pString, vPos, Color);
}

CFont* Engine::CFontMgr::FindFont(const _tchar* pFontKey)
{
	MAPFONT::iterator iter = find_if(m_mapFont.begin(), m_mapFont.end(), CTagFinder(pFontKey));

	if (iter == m_mapFont.end())
		return NULL;

	return iter->second;
}

void Engine::CFontMgr::Free(void)
{
	for_each(m_mapFont.begin(), m_mapFont.end(), CReleasePair());
	m_mapFont.clear();
}
