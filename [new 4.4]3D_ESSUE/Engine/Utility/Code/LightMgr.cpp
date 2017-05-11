#include "LightMgr.h"
#include "Light.h"

USING(Engine)
IMPLEMENT_SINGLETON(CLightMgr)

CLightMgr::CLightMgr(void)
{
}

CLightMgr::~CLightMgr(void)
{
}
HRESULT Engine::CLightMgr::Init_Light(LPDIRECT3DDEVICE9 pDevice, const D3DLIGHT9* pLightInfo)
{
	CLight* pLight = CLight::Create(pDevice, pLightInfo);
	if (pLight == NULL)
		return E_FAIL;

	m_LightList.push_back(pLight);

	return S_OK;
}

void Engine::CLightMgr::Render(LPD3DXEFFECT pEffect)
{
	LIGHTLIST::iterator	iter = m_LightList.begin();
	LIGHTLIST::iterator	iter_end = m_LightList.end();

	for( ; iter != iter_end; ++iter)
	{
		(*iter)->Render(pEffect);
	}
}

void Engine::CLightMgr::Free(void)
{
	for_each(m_LightList.begin(), m_LightList.end(), CReleaseSingle());

	m_LightList.clear();
}

