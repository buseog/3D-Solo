#include "Camera.h"

USING(Engine)


Engine::CCamera::CCamera( LPDIRECT3DDEVICE9 pDevice )
: m_pDevice(pDevice)
{
	m_pDevice->AddRef();
}

Engine::CCamera::~CCamera( void )
{

}

HRESULT Engine::CCamera::Initialize( void )
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pDevice->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFovy, m_fAspect, m_fNear, m_fFar);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

_int Engine::CCamera::Update( const _float& fDelta )
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pDevice->SetTransform(D3DTS_VIEW, &m_matView);

	return 0;
}

void Engine::CCamera::Free( void )
{
	Engine::Safe_Release(m_pDevice);
}
