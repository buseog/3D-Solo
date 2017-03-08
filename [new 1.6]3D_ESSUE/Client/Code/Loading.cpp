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

	//// Texture
	lstrcpy(m_szString, L"Texture Loading...");
	// Terrain
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", Engine::CTexture::TEX_NORMAL, 2);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Terrain", pComponent);
	// SkyBox
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/SkyBox/Burger%d.dds", Engine::CTexture::TEX_CUBE, 4);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_SkyBox", pComponent);
	

	//// Buffer
	lstrcpy(m_szString, L"Buffer Loading...");

	//// Mesh
	lstrcpy(m_szString, L"Mesh Loading...");
	//Terrain
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Map/BossMap/", L"BossMap.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_BossMap", pComponent);
	// Stone
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/TombStone/", L"TombStone.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Stone", pComponent);
	// LongBlade
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Weapon/LongBlade/", L"TitanLongBlade.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_LongBlade", pComponent);

	// Player
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/Arisha/", L"Arisha.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Player", pComponent);


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

