#include "Mesh.h"

USING(Engine)

Engine::CMesh::CMesh( LPDIRECT3DDEVICE9 pDevice )
: m_pDevice(pDevice)
, m_bClone(false)
{
	m_pDevice->AddRef();
}

Engine::CMesh::CMesh( const CMesh& rhs )
: m_pDevice(rhs.m_pDevice)
, m_bClone(true)
{
	m_pDevice->AddRef();
}

Engine::CMesh::~CMesh( void )
{

}

void Engine::CMesh::Free( void )
{
	CComponent::Free();

	Engine::Safe_Release(m_pDevice);
}
