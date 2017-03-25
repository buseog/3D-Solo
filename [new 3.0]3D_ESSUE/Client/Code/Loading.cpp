#include "StdAfx.h"
#include "Loading.h"
#include "Export_Engine.h"

unsigned int APIENTRY CLoading::ThreadMain( void* pArg )
{
	CLoading* pLoading = (CLoading*)pArg;

	_uint iFlag = 0;

	EnterCriticalSection(pLoading->GetCriticalSection());

	switch (pLoading->GetLoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_Stage();
		break;

	case LOADING_BOSS:
		break;
	}

	LeaveCriticalSection(pLoading->GetCriticalSection());

	return iFlag;
}

CLoading::CLoading( LPDIRECT3DDEVICE9 pDevice )
: m_pDevice(pDevice)
, m_bComplete(false)
, m_iMaxLoad(32)
, m_iCntLoad(0)
{
	m_pDevice->AddRef();
	ZeroMemory(m_szString, sizeof(_tchar) * 128);
}

CLoading::~CLoading( void )
{

}

HRESULT CLoading::Initialize( LOADINGID eID )
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadMain, this, 0, NULL);

	m_eID = eID;

	return S_OK;
}

_uint CLoading::Loading_Stage( void )
{
	Engine::CComponent* pComponent = NULL;

	//// Texture ===================================
	lstrcpy(m_szString, L"Texture Loading...");
	// SkyBox
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/SkyBox/SkyBox%d.dds", Engine::CTexture::TEX_CUBE);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_SkyBox", pComponent);
	// LongBladeTrail
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/LongBladeTrail/LongBlade_Trail%d.png", Engine::CTexture::TEX_NORMAL, 6);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_LongBladeTrail", pComponent);
	
	m_iCntLoad += 2;
	//// Buffer ====================================
	lstrcpy(m_szString, L"Buffer Loading...");
	// TrainTex
	pComponent = Engine::CTrainTex::Create(m_pDevice);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Buffer_Train", pComponent);
	// ShaderBuffer
	pComponent = Engine::CShaderBuffer::Create(m_pDevice, 800, 600);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Buffer_Shader", pComponent);

	//// Mesh =========================================
	lstrcpy(m_szString, L"Mesh Loading...");
	// Map
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Map/BossMap/", L"BossMap.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_BossMap", pComponent);
	// LongBlade
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Item/", L"TitanLongBlade.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_LongBlade", pComponent);
	// BossSword
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Item/", L"BossSword.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_BossSword", pComponent);
	// BossShield
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Item/", L"BossShield.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_BossShield", pComponent);
	m_iCntLoad += 8;

	MyMat matIden, matScale, matRot, matTrans;

	// Player
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/Arisha2/", L"Arisha.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Player", pComponent);
	// Boss
	D3DXMatrixRotationY(&matRot, D3DXToRadian(180.f));
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/Boss/", L"Boss.X", &matRot);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Boss", pComponent);
	//// Intro
	//D3DXMatrixRotationY(&matRot, D3DXToRadian(180.f));
	//D3DXMatrixTranslation(&matTrans, 0.f, 0.f, -700.f);
	//pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/Intro/", L"BossIntro.X", &(matRot * matTrans));
	//if (pComponent == NULL)
	//	return E_FAIL;
	//Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Intro", pComponent);
	m_iCntLoad += 20;


	//// Shader =========================================
	lstrcpy(m_szString, L"Shader Loading...");
	// Shader StaticMesh
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_Mesh.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_Mesh", pComponent);
	m_iCntLoad += 2;
	// Shader Effect
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_Effect.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_Effect", pComponent);
	m_iCntLoad += 2;
	// Shader Blur
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_Blur.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_Blur", pComponent);
	m_iCntLoad += 2;



	///// End ===============================================
	m_bComplete = true;
	lstrcpy(m_szString, L"Loading Complete...");

	return 0;
}

CRITICAL_SECTION* CLoading::GetCriticalSection( void )
{
	return &m_Crt;
}

CLoading::LOADINGID CLoading::GetLoadingID( void )
{
	return m_eID;
}

_bool CLoading::GetComplete( void )
{
	return m_bComplete;
}

_tchar* CLoading::GetString( void )
{
	return m_szString;
}
void CLoading::Free( void )
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	::Safe_Release(m_pDevice);
}

CLoading* CLoading::Create( LPDIRECT3DDEVICE9 pDevice, LOADINGID eID )
{
	CLoading* pLoading = new CLoading(pDevice);

	if (FAILED(pLoading->Initialize(eID)))
	{
		MSG_BOX("Loading Created Failed");
		::Safe_Release(pLoading);
	}

	return pLoading;
}

