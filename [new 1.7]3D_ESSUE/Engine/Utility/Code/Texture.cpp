#include "Texture.h"

USING(Engine)

Engine::CTexture::CTexture(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
{
	m_pDevice->AddRef();
}

Engine::CTexture::CTexture(const CTexture& rhs)
: m_pDevice(rhs.m_pDevice)
{
	m_pDevice->AddRef();

	_uint ivecSize = rhs.m_vecTexture.size();
	m_vecTexture.reserve(ivecSize);

	m_vecTexture = rhs.m_vecTexture;

	for (_uint i = 0; i < ivecSize; ++i)
		m_vecTexture[i]->AddRef();
}

Engine::CTexture::~CTexture(void)
{

}

HRESULT Engine::CTexture::Initialize(const _tchar* pPath, TEXTURETYPE eType, const _uint& iCnt)
{
	m_vecTexture.reserve(iCnt);


	IDirect3DBaseTexture9*			pTexture = NULL;

	for(_uint i = 0; i < iCnt; ++i)
	{
		TCHAR			szFileName[128] = L"";

		wsprintf(szFileName, pPath, i);	

		switch(eType)
		{
		case TEX_NORMAL:
			if(FAILED(D3DXCreateTextureFromFile(m_pDevice, szFileName, (LPDIRECT3DTEXTURE9*)&pTexture)))
				return E_FAIL;
			break;
		case TEX_CUBE:
			if(FAILED(D3DXCreateCubeTextureFromFile(m_pDevice, szFileName, (LPDIRECT3DCUBETEXTURE9*)&pTexture)))
				return E_FAIL;
			break;
		}

		m_vecTexture.push_back(pTexture);
	}
	return S_OK;
}

void Engine::CTexture::Render(const _uint& iIndex)
{
	if (m_vecTexture.size() < iIndex)
		return;

	m_pDevice->SetTexture(0, m_vecTexture[iIndex]);
}

void Engine::CTexture::Free(void)
{
	Engine::CComponent::Free();

	Engine::Safe_Release(m_pDevice);

	for (_uint i = 0; i < m_vecTexture.size(); ++i)
	{
		Engine::Safe_Release(m_vecTexture[i]);
	}
	m_vecTexture.clear();

}


Engine::CComponent* Engine::CTexture::Clone(void)
{
	return new CTexture(*this);
}

CTexture* Engine::CTexture::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pPath, TEXTURETYPE eType, const _uint& iCnt /*= 1*/)
{
	CTexture* pTexture = new CTexture(pDevice);

	if (FAILED(pTexture->Initialize(pPath, eType, iCnt)))
	{
		MSG_BOX("CTexture Created Failed");
		Engine::Safe_Release(pTexture);
	}

	return pTexture;
}
