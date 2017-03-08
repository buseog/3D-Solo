#include "Transform.h"

USING(Engine)

Engine::CTransform::CTransform(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
{
	m_pDevice->AddRef();

	ZeroMemory(m_vInfo, sizeof(D3DXVECTOR3) * INFO_END);
	D3DXMatrixIdentity(&m_matWorld);

	m_vInfo[INFO_SCALE] = MyVec3(1.f, 1.f, 1.f);
}

Engine::CTransform::CTransform(const CTransform& rhs)
: m_pDevice(rhs.m_pDevice)
, m_matWorld(rhs.m_matWorld)
{
	m_pDevice->AddRef();
	memcpy(m_vInfo, rhs.m_vInfo, sizeof(MyVec3) * INFO_END);
}

Engine::CTransform::~CTransform(void)
{

}

HRESULT Engine::CTransform::Initialize(void)
{
	return S_OK;
}

void Engine::CTransform::Update(void)
{
	D3DXMATRIX matScale, matRotX, matRotY, matRotZ, matTrans;

	D3DXMatrixScaling(&matScale, m_vInfo[INFO_SCALE].x, m_vInfo[INFO_SCALE].y, m_vInfo[INFO_SCALE].z);
	D3DXMatrixRotationX(&matRotX, m_vInfo[INFO_ANGLE].x);
	D3DXMatrixRotationY(&matRotY, m_vInfo[INFO_ANGLE].y);
	D3DXMatrixRotationZ(&matRotZ, m_vInfo[INFO_ANGLE].z);
	D3DXMatrixTranslation(&matTrans, m_vInfo[INFO_POS].x, m_vInfo[INFO_POS].y, m_vInfo[INFO_POS].z);

	m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;
}

void Engine::CTransform::SetInfomation(INFORMATION eInfo, const D3DXVECTOR3* pVec)
{
	m_vInfo[eInfo] = *pVec;
}

void Engine::CTransform::SetWorld(void)
{
	if (m_pDevice == NULL)
		return;

	m_pDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
}

void Engine::CTransform::Free(void)
{
	Engine::Safe_Release(m_pDevice);
}

CTransform* Engine::CTransform::Clone(void)
{
	return new CTransform(*this);
}

CTransform* Engine::CTransform::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CTransform* pTransform = new CTransform(pDevice);

	if (FAILED(pTransform->Initialize()))
	{
		MSG_BOX("Transform Created Failed");
		Safe_Release(pTransform);
	}

	return pTransform;
}
