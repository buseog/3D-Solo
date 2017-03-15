#include "Transform.h"

USING(Engine)

Engine::CTransform::CTransform(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
, m_pParent(NULL)
{
	m_pDevice->AddRef();

	ZeroMemory(m_vInfo, sizeof(D3DXVECTOR3) * INFO_END);
	D3DXMatrixIdentity(&m_matWorld);

	m_vInfo[INFO_SCALE] = MyVec3(1.f, 1.f, 1.f);
}

Engine::CTransform::CTransform(const CTransform& rhs)
: m_pDevice(rhs.m_pDevice)
, m_matWorld(rhs.m_matWorld)
, m_pParent(rhs.m_pParent)
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

	if (m_pParent != NULL)
		m_matWorld *= *m_pParent;
}

void Engine::CTransform::SetInfomation(INFORMATION eInfo, const D3DXVECTOR3* pVec)
{
	m_vInfo[eInfo] = *pVec;
}

void Engine::CTransform::SetTransform(void)
{
	if (m_pDevice == NULL)
		return;

	m_pDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
}

void Engine::CTransform::SetParentMatrix(MyMat* pMat )
{
	m_pParent = pMat;
}

void Engine::CTransform::SetRotation( ANGLEID eAngle, const _float& fAngle )
{
	switch (eAngle)
	{
	case ANGLE_X:
		m_vInfo[INFO_ANGLE].x = fAngle;
		break;
	case ANGLE_Y:
		m_vInfo[INFO_ANGLE].y = fAngle;
		break;
	case ANGLE_Z:
		m_vInfo[INFO_ANGLE].z = fAngle;
		break;
	}
	
}

void Engine::CTransform::Move( const _float& fSpeed )
{
	MyMat matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0 ,&matView);

	MyVec3 vLook;
	memcpy(&vLook, &matView.m[2][0], sizeof(MyVec3));
	vLook.y = 0.f;
	D3DXVec3Normalize(&vLook, &vLook);

	m_vInfo[INFO_POS] += vLook * fSpeed;
	
	MyVec3 vMyLook;
	memcpy(&vMyLook, &m_matWorld.m[2][0], sizeof(MyVec3));
	D3DXVec3Normalize(&vMyLook, &vMyLook);
	
	MyVec3	vUp;
	D3DXVec3Cross(&vUp, &vMyLook, &vLook);
	_float fAngleSpeed = 0.f;

	if (vUp.y > 0)
		fAngleSpeed = 1.f;
	else
		fAngleSpeed = -1.f;

	_float fAngle = D3DXVec3Dot(&MyVec3(0.f, 0.f, 1.f), &vLook);
	
	if (fAngle > 0)
		fAngle = acosf(fAngle);
	else
		fAngle = 2 * D3DX_PI - acosf(fAngle);

	if (m_vInfo[INFO_ANGLE].y >= fAngle * 0.9f && m_vInfo[INFO_ANGLE].y <= fAngle * 1.1f)
		return;

	m_vInfo[INFO_ANGLE].y += D3DXToRadian(90.f) * fAngleSpeed * fSpeed;
}

void Engine::CTransform::SideMove( const _float& fSpeed )
{
	MyMat matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0 ,&matView);

	MyVec3 vRight;
	memcpy(&vRight, &matView.m[0][0], sizeof(MyVec3));
	D3DXVec3Normalize(&vRight, &vRight);
	vRight.y = 0.f;

	m_vInfo[INFO_POS] += vRight * fSpeed;

	MyVec3 vMyLook;
	memcpy(&vMyLook, &m_matWorld.m[2][0], sizeof(MyVec3));
	D3DXVec3Normalize(&vMyLook, &vMyLook);

	MyVec3	vUp;
	D3DXVec3Cross(&vUp, &vMyLook, &vRight);
	_float fAngleSpeed = 0.f;

	if (vUp.y > 0)
		fAngleSpeed = 1.f;
	else
		fAngleSpeed = -1.f;

	_float fAngle = D3DXVec3Dot(&MyVec3(0.f, 0.f, 1.f), &vRight);

	if (fAngle > 0)
		fAngle = acosf(fAngle);
	else
		fAngle = 2 * D3DX_PI - acosf(fAngle);

	if (m_vInfo[INFO_ANGLE].y >= fAngle * 0.9f && m_vInfo[INFO_ANGLE].y <= fAngle * 1.1f)
		return;

	m_vInfo[INFO_ANGLE].y += D3DXToRadian(90.f) * fAngleSpeed * fSpeed;
}

MyMat* Engine::CTransform::GetWorldMatrix( void )
{
	return &m_matWorld;
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
