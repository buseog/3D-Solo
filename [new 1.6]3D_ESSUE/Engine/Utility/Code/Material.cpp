#include "Material.h"

USING(Engine)

CMaterial::CMaterial( LPDIRECT3DDEVICE9 pDevice )
: m_pDevice(pDevice)
{
	ZeroMemory(&m_MtrInfo, sizeof(D3DMATERIAL9));

	m_pDevice->AddRef();
}

Engine::CMaterial::CMaterial(const CMaterial& rhs)
: m_pDevice(rhs.m_pDevice)
{
	memcpy(&m_MtrInfo, &rhs.m_MtrInfo, sizeof(D3DMATERIAL9));
	m_pDevice->AddRef();
}

CMaterial::~CMaterial( void )
{

}

HRESULT CMaterial::Initialize( const D3DXCOLOR& Diffuse, const D3DXCOLOR& Ambient, const D3DXCOLOR& Specular, const D3DXCOLOR& Emissive, const _float& fPower )
{
	m_MtrInfo.Diffuse = Diffuse;
	m_MtrInfo.Ambient = Ambient;
	m_MtrInfo.Specular = Specular;
	m_MtrInfo.Emissive = Emissive;
	m_MtrInfo.Power = fPower;

	return S_OK;
}

void Engine::CMaterial::SetMaterial( void )
{
	m_pDevice->SetMaterial(&m_MtrInfo);
}

void Engine::CMaterial::ChangeMaterial(MATERIALID eID, void* pValue)
{
	if (eID == MTR_POWER)
		m_MtrInfo.Power = *(_float*)pValue;
	else
		*((D3DCOLORVALUE*)&m_MtrInfo + eID) = *(D3DCOLORVALUE*)pValue;
}

void CMaterial::Free( void )
{
	Engine::Safe_Release(m_pDevice);
}

CComponent* CMaterial::Clone( void )
{
	return new CMaterial(*this);
}

CMaterial* CMaterial::Create( LPDIRECT3DDEVICE9 pDevice, const D3DXCOLOR& Diffuse, const D3DXCOLOR& Ambient, const D3DXCOLOR& Specular, const D3DXCOLOR& Emissive, const _float& fPower )
{
	CMaterial* pMaterial = new CMaterial(pDevice);

	if (FAILED(pMaterial->Initialize(Diffuse, Ambient, Specular, Emissive, fPower)))
	{
		MSG_BOX("Material Created Failed");
	}

	return pMaterial;
}
