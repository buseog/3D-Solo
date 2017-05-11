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
, m_iMaxLoad(64)
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
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/SkyBox/SkyBox%d.dds", Engine::CTexture::TEX_CUBE, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_SkyBox", pComponent);
	// LongBladeTrail
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/LongBladeTrail/LongBlade_Trail%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 6);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_LongBladeTrail", pComponent);
	// Satellite
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Satellite/Satellite%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Satellite", pComponent);
	// MagicCIrcle
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/MagicCircle%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 2);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_MagicCircle", pComponent);
	// Ring Outer
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring_outer_wind%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 7);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Ring_Outer_Wind", pComponent);
	// Ring Outer_n
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring_outer_wind02_n.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Ring_Outer_Wind_n", pComponent);
	// SlashPoint
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/SlashPoint%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_SlashPoint", pComponent);
	
	// YellowFlare
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/yellowflare%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 8);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_YellowFlare", pComponent);
	// LightGlow
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/light_glow%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 5);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_LightGlow", pComponent);
	// Ring
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 8);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Ring", pComponent);
	// RingDisk
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring_disk%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_RingDisk", pComponent);
	// IceLight
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/evy_icelight_%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 8);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_IceLight", pComponent);
	// AmingPoint
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/aming_point_big%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 7);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_AmingPoint", pComponent);
	// PartiSpray
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/parti_Spray%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_PartiSpray", pComponent);
	// EfxRipple
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/efx_ripple%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_efxRipple", pComponent);
	// Smoke
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/smoke_%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 6);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Smoke", pComponent);
	// WIndBladeB
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/wind_blade_b%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_WindBlade", pComponent);
	// WIndBladeSmall
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/wind_blade_small%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_WindBladeSmall", pComponent);	
	// Dust
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/dust%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 4);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Dust", pComponent);
	// Dust2
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/dust2_%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 2);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Dust2", pComponent);
	// fire_seed_C_a_s
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fire_seed_C_a_s%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Fire_Seed_C_a_s", pComponent);
	// fire_seed_D_a_s
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fire_seed_D_a_s%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Fire_Seed_D_a_s", pComponent);
	// fire_seed_D_b_s
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fire_seed_D_b_s%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Fire_Seed_D_b_s", pComponent);
	// DustWhite
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/dust_white%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 5);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_DustWhite", pComponent);
	// DarkSmoke
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/dark_smoke%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_DarkSmoke", pComponent);
	// DarkCell
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/dark_cell_%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 5);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_DarkCell", pComponent);
	// FireFlare
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fireflare%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_FireFlare", pComponent);
	// Bomb
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/bomb%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 5);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Bomb", pComponent);
	// Blunt
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/blunt%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 7);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Blunt", pComponent);
	// BluntYellow
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/blunt_yellow%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 11);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_BluntYellow", pComponent);
	// CriticalEhen
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/critical_ehen%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 4);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_CriticalEhen", pComponent);
	// Lightning
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/lightning%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 8);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Lightning", pComponent);
	// FireSpark
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fire_spark%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 7);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_FireSpark", pComponent);
	// FireStill
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/fire_still%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_FireStill", pComponent);
	// RingFire
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring_fire%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 2);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_RingFire", pComponent);
	// RingFireDark
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring_fire_dark%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_RingFireDark", pComponent);
	// LightRay
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/light_ray%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_LightRay", pComponent);
	// ShipGray
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ship_gray%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 2);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_ShipGray", pComponent);
	// RingLine
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/ring_line%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 5);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_RingLine", pComponent);
	// SimpleAura
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/simple_aura%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 3);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_SimpleAura", pComponent);
	// EfxWisp
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/efx_wisp%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 3);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_EfxWisp", pComponent);
	// EfxSwordSharp
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/efx_sword_sharp%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 3);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_SwordSharp", pComponent);
	// Light
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/light%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Light", pComponent);
	// Light_n
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/light_n%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Light_n", pComponent);
	// ShapeFire
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/shape_fire%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 4);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_ShapeFire", pComponent);
	// Stone
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/stone%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Stone", pComponent);
	// Stone_01b
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/stone_01b%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Stone01b", pComponent);
	// Ice
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/Ice%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 3);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Ice", pComponent);
	// mercury_sneezn
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/mercury_sneezn%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 3);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Mercury_Sneezn", pComponent);
	// Flash
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/Effect/Flash%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Flash", pComponent);
	// HPBar
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Texture/UI/Hp%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad, 2);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_HpBar", pComponent);

	//// Buffer ====================================
	lstrcpy(m_szString, L"Buffer Loading...");
	// TrailTex
	pComponent = Engine::CTrailTex::Create(m_pDevice);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Buffer_Trail", pComponent);
	++m_iCntLoad;
	// ShaderBuffer
	pComponent = Engine::CShaderBuffer::Create(m_pDevice, 800, 600);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Buffer_Shader", pComponent);
	++m_iCntLoad;

	//// Mesh =========================================
	lstrcpy(m_szString, L"Mesh Loading...");
	// Map
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Map/BossMap2/", L"BossMap.X", &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_BossMap", pComponent);
	// Waterfall
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Map/BossMap2/", L"Waterfall.X", &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Waterfall", pComponent);
	// Water
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Map/BossMap2/", L"Mountain.X", &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Mountain", pComponent);
	// Mountain
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Map/BossMap2/", L"Water.X", &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Water", pComponent);
	// BossShield
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Item/", L"BossShield.X", &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_BossShield", pComponent);
	// Mountain_n
	pComponent = Engine::CTexture::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Map/BossMap2/Mountain_n%d.tga", Engine::CTexture::TEX_NORMAL, &m_iCntLoad);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Texture_Mountain_n", pComponent);

	MyMat matIden, matScale, matRot, matTrans;

	// Player
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/Arisha/", L"Arisha.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Player", pComponent);
	m_iCntLoad += 13;
	// LongBlade
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Item/", L"TitanLongBlade.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_LongBlade", pComponent);
	m_iCntLoad += 1;
	// DarkNight
	/*pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/DarkNight/", L"DarkNight.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_DarkNight", pComponent);*/
	// Boss
	D3DXMatrixRotationY(&matRot, D3DXToRadian(180.f));
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/Boss/", L"Boss.X", &matRot);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Boss", pComponent);
	m_iCntLoad += 20;
	// BossSword
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/StaticMesh/Item/", L"BossSword.X");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_BossSword", pComponent);
	m_iCntLoad += 1;
	//// Intro
	D3DXMatrixRotationY(&matRot, D3DXToRadian(180.f));
	D3DXMatrixTranslation(&matTrans, 0.f, 0.f, -800.f);
	pComponent = Engine::CDynamicMesh::Create(m_pDevice, L"../Bin/Resource/Mesh/DynamicMesh/Intro/", L"BossIntro.X", &(matRot * matTrans));
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Mesh_Intro", pComponent);
	m_iCntLoad += 20;


	//// Shader =========================================
	lstrcpy(m_szString, L"Shader Loading...");
	// Shader Mesh
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_Mesh.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_Mesh", pComponent);
	++m_iCntLoad;
	// Shader Effect
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_Effect.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_Effect", pComponent);
	++m_iCntLoad;
	// Shader Blur
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_Blur.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_Blur", pComponent);
	++m_iCntLoad;
	// Shader SkyBox
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_SkyBox.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_SkyBox", pComponent);
	++m_iCntLoad;
	// Shader Particle
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_Particle.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_Particle", pComponent);
	++m_iCntLoad;
	// Shader Environ
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_Environ.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_Environ", pComponent);
	++m_iCntLoad;
	// Shader Environ
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_UI.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STAGE, L"Component_Shader_UI", pComponent);
	++m_iCntLoad;



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

