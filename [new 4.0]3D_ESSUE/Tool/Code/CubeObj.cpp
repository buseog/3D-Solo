#include "StdAfx.h"
#include "CubeObj.h"


CCubeObj::CCubeObj( LPDIRECT3DDEVICE9 pDevice )
: Engine::CGameObject(pDevice)
, m_pTransformCom(NULL)
, m_pBufferCom(NULL)
{

}

CCubeObj::~CCubeObj( void )
{

}

HRESULT CCubeObj::Initialize( const MyVec3* pPos )
{
	m_pTransformCom = (Engine::CTransform*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Transform");
	if (m_pTransformCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_DO, L"Com_Transform", m_pTransformCom)))
		return E_FAIL;	

	m_pBufferCom = (Engine::CCubeCol*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Buffer_CubeCol");
	if (m_pBufferCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(CGameObject::UPDATE_NONE, L"Com_Buffer", m_pBufferCom)))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, pPos);
	m_pTransformCom->Update();

	return S_OK;
}

_int CCubeObj::Update( const _float& fDelta )
{
	Engine::CGameObject::Update(fDelta);

	return 0;
}

void CCubeObj::Render( void )
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pDevice->SetTexture(0, NULL);
	m_pTransformCom->SetUpDevice();
	m_pBufferCom->SetColor(m_vColor);
	m_pBufferCom->Render();

	m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CCubeObj::Free( void )
{
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pBufferCom);

	Engine::CGameObject::Free();
}

CCubeObj* CCubeObj::Create( LPDIRECT3DDEVICE9 pDevice, const MyVec3* pPos )
{
	CCubeObj* pCubeObj = new CCubeObj(pDevice);

	if (FAILED(pCubeObj->Initialize(pPos)))
	{
		MSG_BOX("CubeObj Created Failed");
		Engine::Safe_Release(pCubeObj);
	}
	return pCubeObj;
}


void CCubeObj::SetColor( D3DXCOLOR vColor )
{
	m_vColor = vColor;
}
