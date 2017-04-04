#include "Font.h"

USING(Engine)

Engine::CFont::CFont(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
{
	m_pDevice->AddRef();
}

Engine::CFont::~CFont(void)
{
	
}

HRESULT Engine::CFont::InitFont(const _tchar* pFaceName, const _uint& iHeight, const _uint& iWidth, const _uint& iWeight)
{
	D3DXFONT_DESC		Font_Desc;
	ZeroMemory(&Font_Desc, sizeof(D3DXFONT_DESC));

	Font_Desc.Height = m_iHeight = iHeight;
	Font_Desc.Width  = m_iWidth  = iWidth;
	Font_Desc.Weight = iWeight;
	lstrcpy(Font_Desc.FaceName, pFaceName);
	Font_Desc.CharSet = HANGUL_CHARSET;

	D3DXCreateFontIndirect(m_pDevice, &Font_Desc, &m_pFont);

	return S_OK;
}

void Engine::CFont::Render(const _tchar* pString, const MyVec3& vPos, D3DXCOLOR Color)
{
	RECT rc = { 
		long(vPos.x), 
		long(vPos.y), 
		long(vPos.x) + m_iWidth * lstrlen(pString),
		long(vPos.y) + m_iHeight
	};

	m_pFont->DrawTextW(NULL, pString, lstrlen(pString), &rc, DT_NOCLIP, Color);

}

CFont* Engine::CFont::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pFaceName, const _uint& iHeight, const _uint& iWidth, const _uint& iWeight)
{
	CFont*		pFont = new CFont(pDevice);

	if (FAILED(pFont->InitFont(pFaceName, iHeight, iWidth, iWeight)))
	{
		MSG_BOX("Font Created Failed");
		Engine::Safe_Release(pFont);
	}

	return pFont;
}

void Engine::CFont::Free(void)
{
	Engine::Safe_Release(m_pFont);
	Engine::Safe_Release(m_pDevice);
}

