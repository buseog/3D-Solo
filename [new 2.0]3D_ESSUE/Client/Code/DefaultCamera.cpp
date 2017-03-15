#include "StdAfx.h"
#include "DefaultCamera.h"

#include "Export_Engine.h"

CDefaultCamera::CDefaultCamera( LPDIRECT3DDEVICE9 pDevice )
: CCamera(pDevice)
, m_pTarget(NULL)
, m_fDistance(0.f)
{
	ZeroMemory(m_fAngle, sizeof(_float) * ANGLE_END);
}

CDefaultCamera::~CDefaultCamera( void )
{

}

HRESULT CDefaultCamera::Initialize(MyMat* pTarget, const MyVec3* pUp, _float fFovy, _float fAspect, _float fNear, _float fFar )
{
	m_pTarget = pTarget;
	MyVec3 vPos;
	memcpy(&vPos, &m_pTarget->m[3][0], sizeof(MyVec3));

	m_fDistance = 40.f;

	m_vAt = vPos;
	m_vEye = -g_vLook * m_fDistance + vPos;
	m_vUp = *pUp;

	m_fFovy = fFovy;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;
	if(FAILED(Engine::CCamera::Initialize()))
		return E_FAIL;

	return S_OK;
}

_int CDefaultCamera::Update( const _float& fDelta )
{
	SetUpCamera();

	KeyCheck(fDelta);

	POINT			ptCenter = { WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptCenter);

	SetCursorPos(ptCenter.x, ptCenter.y);

	_int iExit = Engine::CCamera::Update(fDelta);

	return iExit;
}

void CDefaultCamera::KeyCheck( const _float& fDelta )
{
	MyMat			matWorld;
	D3DXMatrixInverse(&matWorld, NULL, &m_matView);

	_long	dwMouseMove = 0;


	if(dwMouseMove = Engine::GetDIMouseMove(Engine::CInputDevice::DIMS_X))
	{
		m_fAngle[ANGLE_Y] += dwMouseMove / 10.f;
	}
}

void CDefaultCamera::SetUpCamera( void )
{
	MyVec3 vPos;

	memcpy(&vPos, &m_pTarget->m[3][0], sizeof(MyVec3));

	m_vEye = -g_vLook * m_fDistance;

	MyMat	matRotAxisX, matRotAxisY;
	D3DXMatrixRotationAxis(&matRotAxisX, &MyVec3(1.f, 0.f, 0.f), D3DXToRadian(30.f));
	D3DXVec3TransformNormal(&m_vEye, &m_vEye, &matRotAxisX);

	D3DXMatrixRotationAxis(&matRotAxisY, &MyVec3(0.f, 1.f, 0.f), D3DXToRadian(m_fAngle[ANGLE_Y]));
	D3DXVec3TransformNormal(&m_vEye, &m_vEye, &matRotAxisY);

	m_vAt = vPos;
	m_vEye += vPos;
}

void CDefaultCamera::SetTarget(MyMat* pMat )
{
	m_pTarget = pMat;
}

void CDefaultCamera::Free( void )
{
	Engine::CCamera::Free();
}

CDefaultCamera* CDefaultCamera::Create(LPDIRECT3DDEVICE9 pDevice,  MyMat* pTarget, const MyVec3* pUp, _float fFovy /*= D3DXToRadian(60.0f)*/, _float fAspect /*= _float(WINCX) / WINCY*/, _float fNear /*= 0.1f*/, _float fFar /*= 1000.f*/ )
{
	CDefaultCamera* pCamera = new CDefaultCamera(pDevice);

	if (FAILED(pCamera->Initialize(pTarget, pUp, fFovy, fAspect, fNear, fFar)))
	{
		MSG_BOX("DynamicCamera Created Failed");
		::Safe_Release(pCamera);
	}

	return pCamera;
}
