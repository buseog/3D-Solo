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
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/LongBladeTrail/LongBlade_Trail%d.tga", Engine::CTexture::TEX_NORMAL, 6);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_LongBladeTrail", pComponent);
	// Satellite
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Satellite/Satellite%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Satellite", pComponent);
	// MagicCIrcle
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/MagicCircle%d.tga", Engine::CTexture::TEX_NORMAL, 2);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_MagicCircle", pComponent);
	// Ring Outer
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring_outer_wind%d.tga", Engine::CTexture::TEX_NORMAL, 7);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Ring_Outer_Wind", pComponent);
	// Ring Outer_n
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring_outer_wind02_n.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Ring_Outer_Wind_n", pComponent);
	// SlashPoint
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/SlashPoint%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_SlashPoint", pComponent);
	
	// YellowFlare
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/yellowflare%d.tga", Engine::CTexture::TEX_NORMAL, 8);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_YellowFlare", pComponent);
	// LightGlow
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/light_glow%d.tga", Engine::CTexture::TEX_NORMAL, 5);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_LightGlow", pComponent);
	// Ring
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring%d.tga", Engine::CTexture::TEX_NORMAL, 8);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Ring", pComponent);
	// RingDisk
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring_disk%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_RingDisk", pComponent);
	// IceLight
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/evy_icelight_%d.tga", Engine::CTexture::TEX_NORMAL, 8);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_IceLight", pComponent);
	// AmingPoint
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/aming_point_big%d.tga", Engine::CTexture::TEX_NORMAL, 7);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_AmingPoint", pComponent);
	// PartiSpray
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/parti_Spray%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_PartiSpray", pComponent);
	// EfxRipple
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/efx_ripple%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_efxRipple", pComponent);
	// Smoke
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/smoke_%d.tga", Engine::CTexture::TEX_NORMAL, 6);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Smoke", pComponent);
	// WIndBladeB
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/wind_blade_b%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_WindBlade", pComponent);
	// Dust
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/dust%d.tga", Engine::CTexture::TEX_NORMAL, 4);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Dust", pComponent);
	// fire_seed_C_a_s
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fire_seed_C_a_s%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Fire_Seed_C_a_s", pComponent);
	// fire_seed_D_a_s
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fire_seed_D_a_s%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Fire_Seed_D_a_s", pComponent);
	// fire_seed_D_b_s
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fire_seed_D_b_s%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Fire_Seed_D_b_s", pComponent);
	// DustWhite
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/dust_white%d.tga", Engine::CTexture::TEX_NORMAL, 5);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_DustWhite", pComponent);
	// DarkSmoke
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/dark_smoke%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_DarkSmoke", pComponent);
	// DarkCell
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/dark_cell_%d.tga", Engine::CTexture::TEX_NORMAL, 5);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_DarkCell", pComponent);
	// FireFlare
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fireflare%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_FireFlare", pComponent);
	// Bomb
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/bomb%d.tga", Engine::CTexture::TEX_NORMAL, 5);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Bomb", pComponent);
	// Blunt
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/blunt%d.tga", Engine::CTexture::TEX_NORMAL, 7);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Blunt", pComponent);
	// BluntYellow
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/blunt_yellow%d.tga", Engine::CTexture::TEX_NORMAL, 11);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_BluntYellow", pComponent);
	// CriticalEhen
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/critical_ehen%d.tga", Engine::CTexture::TEX_NORMAL, 4);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_CriticalEhen", pComponent);
	// Lightning
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/lightning%d.tga", Engine::CTexture::TEX_NORMAL, 8);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Lightning", pComponent);
	// FireSpark
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fire_spark%d.tga", Engine::CTexture::TEX_NORMAL, 7);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_FireSpark", pComponent);
	// FireStill
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fire_still%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_FireStill", pComponent);
	// RingFire
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring_fire%d.tga", Engine::CTexture::TEX_NORMAL, 2);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_RingFire", pComponent);
	// LightRay
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/light_ray%d.tga", Engine::CTexture::TEX_NORMAL);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_LightRay", pComponent);
	// ShipGray
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ship_gray%d.tga", Engine::CTexture::TEX_NORMAL, 2);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_ShipGray", pComponent);
	// RingLine
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring_line%d.tga", Engine::CTexture::TEX_NORMAL, 5);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_RingLine", pComponent);
	// SimpleAura
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/simple_aura%d.tga", Engine::CTexture::TEX_NORMAL, 3);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_SimpleAura", pComponent);
	// EfxWisp
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/efx_wisp%d.tga", Engine::CTexture::TEX_NORMAL, 3);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_EfxWisp", pComponent);
	
	m_iCntLoad += 2;
	//// Buffer ====================================
	lstrcpy(m_szString, L"Buffer Loading...");
	// TrailTex
	pComponent = Engine::CTrailTex::Create(m_pDevice);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Buffer_Trail", pComponent);
	// ShaderBuffer
	pComponent = Engine::CShaderBuffer::Create(m_pDevice, 800, 600);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Buffer_Shader", pComponent);

	//// Mesh =========================================
	lstrcpy(m_szString, L"Mesh Loading...");
	// Map
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Map/BossMap2/", L"BossMap.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_BossMap", pComponent);
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

	MyMat matIden, matScale, matRot, matTrans;

	// Player
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/Arisha2/", L"Arisha.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Player", pComponent);
	// DarkNight
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/DarkNight/", L"DarkNight.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_DarkNight", pComponent);
	// LongBlade
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Item/", L"TitanLongBlade.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_LongBlade", pComponent);
	// Boss
	/*D3DXMatrixRotationY(&matRot, D3DXToRadian(180.f));
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/Boss/", L"Boss.X", &matRot);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Boss", pComponent);*/
	//// Intro
	//D3DXMatrixRotationY(&matRot, D3DXToRadian(180.f));
	//D3DXMatrixTranslation(&matTrans, 0.f, 0.f, -700.f);
	//pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/Intro/", L"BossIntro.X", &(matRot * matTrans));
	//if (pComponent == NULL)
	//	return E_FAIL;
	//Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Intro", pComponent);
	//m_iCntLoad += 20;


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
	// Shader SkyBox
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_SkyBox.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_SkyBox", pComponent);
	// Shader Particle
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_Particle.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_Particle", pComponent);
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

