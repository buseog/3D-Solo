#include "Light.h"

USING(Engine)

Engine::CLight::CLight(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
, m_pVB(NULL)
, m_pIB(NULL)
{
	m_pDevice->AddRef();
}

Engine::CLight::~CLight(void)
{

}

HRESULT Engine::CLight::Initialize(const D3DLIGHT9* pLightInfo)
{
	m_tLightInfo = *pLightInfo;

	D3DVIEWPORT9		Viewport;
	m_pDevice->GetViewport(&Viewport);

	if(FAILED(m_pDevice->CreateVertexBuffer(sizeof(VTXSCREENTEX) * 4, 0, D3DFVF_VTXSCREENTEX, D3DPOOL_MANAGED, &m_pVB, NULL)))
		return E_FAIL;

	VTXSCREENTEX*				pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = MyVec4(-0.5f, -0.5f, 0.f, 1.f);
	pVertex[0].vTexUV = MyVec2(0.f, 0.f);

	pVertex[1].vPosition = MyVec4(-0.5f + Viewport.Width, -0.5f, 0.f, 1.f);
	pVertex[1].vTexUV = MyVec2(1.f, 0.f);

	pVertex[2].vPosition = MyVec4(-0.5f + Viewport.Width, -0.5f + Viewport.Height, 0.f, 1.f);
	pVertex[2].vTexUV = MyVec2(1.f, 1.f);

	pVertex[3].vPosition = MyVec4(-0.5f, -0.5f + Viewport.Height, 0.f, 1.f);
	pVertex[3].vTexUV = MyVec2(0.0f, 1.0f);

	m_pVB->Unlock();

	if(FAILED(m_pDevice->CreateIndexBuffer(sizeof(INDEX16) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL)))
		return E_FAIL;

	INDEX16*			pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void Engine::CLight::Render(LPD3DXEFFECT pEffect)
{
	_uint	iPassIdx = 0;
	if (m_tLightInfo.Type == D3DLIGHT_DIRECTIONAL)
	{
		iPassIdx = 0;
		pEffect->SetVector("g_vLightDir", &MyVec4(m_tLightInfo.Direction, 0.f));
	}
	else if (m_tLightInfo.Type == D3DLIGHT_DIRECTIONAL)
	{
		iPassIdx = 1;
		pEffect->SetVector("g_vLightPos", &MyVec4(m_tLightInfo.Position, 1.f));
		pEffect->SetFloat("g_fRange", m_tLightInfo.Range);
	}

	pEffect->BeginPass(iPassIdx);

	pEffect->SetVector("g_vLightDir", &MyVec4(m_tLightInfo.Direction, 0.f));
	pEffect->SetVector("g_vLightDiffuse", (MyVec4*)(&m_tLightInfo.Diffuse));
	pEffect->SetVector("g_vLightAmbient", (MyVec4*)(&m_tLightInfo.Ambient));

	MyMat		matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);

	pEffect->SetVector("g_vCamPos", (MyVec4*)&matView.m[3][0]);

	pEffect->SetMatrix("g_matInvProj", &matProj);
	pEffect->SetMatrix("g_matInvView", &matView);

	pEffect->CommitChanges();

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTXSCREENTEX));
	m_pDevice->SetFVF(D3DFVF_VTXSCREENTEX);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
}

void Engine::CLight::Free(void)
{
	Engine::Safe_Release(m_pDevice);

	Engine::Safe_Release(m_pVB);
	Engine::Safe_Release(m_pIB);
}

Engine::CLight* Engine::CLight::Create(LPDIRECT3DDEVICE9 pDevice, const D3DLIGHT9* pLightInfo)
{
	CLight* pLight = new CLight(pDevice);

	if(FAILED(pLight->Initialize(pLightInfo)))
	{
		MSG_BOX("CLight Created Failed");
		Engine::Safe_Release(pLight);
	}

	return pLight;
}
