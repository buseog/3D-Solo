#include "Shader.h"

USING(Engine)

Engine::CShader::CShader( LPDIRECT3DDEVICE9 pDevice )
: m_pDevice(pDevice)
, m_pEffect(NULL)
, m_pErrBuffer(NULL)
{
	m_pDevice->AddRef();
}

Engine::CShader::CShader( const CShader& rhs )
: m_pDevice(rhs.m_pDevice)
, m_pEffect(rhs.m_pEffect)
, m_pErrBuffer(rhs.m_pErrBuffer)
{
	m_pDevice->AddRef();
	m_pEffect->AddRef();

	if (m_pErrBuffer != NULL)
		m_pErrBuffer->AddRef();
}

Engine::CShader::~CShader( void )
{
}

HRESULT Engine::CShader::Initialize( const _tchar* pFilePath )
{
	if(FAILED(D3DXCreateEffectFromFile(m_pDevice, pFilePath, NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pEffect, &m_pErrBuffer)))
	{
		MessageBoxA(NULL, (char*)m_pErrBuffer->GetBufferPointer(), NULL, MB_OK);
		return E_FAIL;		
	}

	if(NULL != m_pErrBuffer)
		MessageBoxA(NULL, (char*)m_pErrBuffer->GetBufferPointer(), NULL, MB_OK);

	return S_OK;
}

void Engine::CShader::Free( void )
{
	Engine::Safe_Release(m_pDevice);
	Engine::Safe_Release(m_pEffect);
	Engine::Safe_Release(m_pErrBuffer);
}

CComponent* Engine::CShader::Clone( void )
{
	return new CShader(*this);
}

CShader* Engine::CShader::Create( LPDIRECT3DDEVICE9 pDevice, const _tchar* pFilePath )
{
	CShader* pShader = new CShader(pDevice);

	if (FAILED(pShader->Initialize(pFilePath)))
	{
		MSG_BOX("Shader Created Failed");
		Engine::Safe_Release(pShader);
	}
	return pShader;
}
