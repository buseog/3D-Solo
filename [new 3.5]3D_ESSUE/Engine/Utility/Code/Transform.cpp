#include "Transform.h"
#include "NaviMgr.h"

USING(Engine)

Engine::CTransform::CTransform(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
, m_pParent(NULL)
, m_dwIndex(0)
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
, m_dwIndex(rhs.m_dwIndex)
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

void Engine::CTransform::SetInfomation(INFORMATION eInfo, const D3DXVECTOR3* pVec, const _uint& iOption)
{
	if (iOption == 0)
		m_vInfo[eInfo] = *pVec;
	else
		m_vInfo[eInfo] += *pVec;
}

void Engine::CTransform::SetUpDevice(void)
{
	if (m_pDevice == NULL)
		return;

	m_pDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
}

void Engine::CTransform::SetUpShader( LPD3DXEFFECT pEffect )
{
	if (pEffect == NULL)
		return;

	pEffect->SetMatrix("g_matWorld", &m_matWorld);
}

void Engine::CTransform::SetParentMatrix(MyMat* pMat )
{
	m_pParent = pMat;
}

void Engine::CTransform::Move( const MyVec3* pDir, const _float& fSpeedDelta )
{
	MyVec3 vMyLook;
	memcpy(&vMyLook, &m_matWorld.m[2][0], sizeof(MyVec3));
	D3DXVec3Normalize(&vMyLook, &vMyLook);

	MyVec3	vUp;
	D3DXVec3Cross(&vUp, &vMyLook, pDir);
	_float fAngleSpeed = 0.f;

	if (vUp.y >= 0)
		fAngleSpeed = 1.f;
	else
		fAngleSpeed = -1.f;

	_float fAngle = D3DXVec3Dot(&MyVec3(0.f, 0.f, 1.f), pDir);

	if (fAngle < 0)
		fAngle = acosf(fAngle);
	else
		fAngle = 2 * D3DX_PI - acosf(fAngle);

	_float fAngleTum = D3DXToRadian(2.f);

	if (m_vInfo[INFO_ANGLE].y >= fAngle - fAngleTum && m_vInfo[INFO_ANGLE].y <= fAngle + fAngleTum)
		return;	

	m_vInfo[INFO_ANGLE].y += D3DXToRadian(10.f) * fAngleSpeed * fSpeedDelta;
	Update();

	MyVec3 vLook;
	memcpy(&vLook, &m_matWorld.m[2][0], sizeof(MyVec3));
	D3DXVec3Normalize(&vLook, &vLook);

	Engine::CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_vInfo[INFO_POS], &MyVec3(vLook * fSpeedDelta), &m_dwIndex);
	//m_vInfo[INFO_POS] += vLook * fSpeedDelta;
}


void Engine::CTransform::Straight( const MyVec3* pDir, const _float& fSpeedDelta )
{
	m_vInfo[INFO_POS] += *pDir * fSpeedDelta;
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
