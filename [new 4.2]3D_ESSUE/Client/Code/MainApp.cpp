#include "StdAfx.h"
#include "MainApp.h"
#include "Logo.h"

#include "Export_Engine.h"
#include <CTime>

CMainApp::CMainApp(void)
: m_pDevice(NULL)
, m_dwCnt(0)
, m_fTimeAcc(0.f)
{
	ZeroMemory(m_szFPS, sizeof(_tchar) * 128);
}

CMainApp::~CMainApp(void)
{
}

HRESULT CMainApp::Initialize(void)
{
	if (FAILED(Ready_Setting()))
		return E_FAIL;

	srand(unsigned(time(NULL)));

	return S_OK;
}

_int CMainApp::Update(const _float& fDelta)
{
	m_fTimeAcc += fDelta;

	Engine::Get_InputDevice()->Update();

	return Engine::Get_Management()->Update(fDelta);
}

void CMainApp::Render(void)
{
	++m_dwCnt;

	if(m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS : %d", m_dwCnt);
		m_fTimeAcc = 0.f;
		m_dwCnt = 0;
	}

	Engine::Get_GraphicDev()->Render_Begin();

	Engine::Get_Management()->Render();

	Engine::Get_FontMgr()->Render(L"Font_Batang", m_szFPS, Engine::MyVec3(0.f, 0.f, 0.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	Engine::Get_GraphicDev()->Render_End();
}

HRESULT CMainApp::Ready_Setting(void)
{
	//// µð¹ÙÀÌ½º
	if (FAILED(Engine::Get_GraphicDev()->InitGraphicDev(g_hWnd, Engine::CGraphicDev::MODE_WIN, WINCX, WINCY)))
		return E_FAIL;

	m_pDevice = Engine::Get_GraphicDev()->GetDevice();
	m_pDevice->AddRef();

	//// Device Input
	if (FAILED(Engine::Get_InputDevice()->Initialize(g_hInst, g_hWnd)))
		return E_FAIL;

	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//// FontMgr
	if (FAILED(Engine::Get_FontMgr()->AddFont(m_pDevice, L"Font_Batang", L"Batang", 20, 20, FW_HEAVY)))
		return E_FAIL;

	//// Component
	if (FAILED(Ready_Component()))
		return E_FAIL;

	//////// Management
	if (FAILED(Engine::Get_Management()->InitManagement()))
		return E_FAIL;

	////// ¾À
	Engine::CScene* pScene = CLogo::Create(m_pDevice);
	if (pScene == NULL)
		return E_FAIL;
	if (FAILED(Engine::Get_Management()->SetScene(pScene)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Component( void )
{
	if (FAILED(Engine::Get_ComponentMgr()->ReserveComponentMgr(SCENE_END)))
		return E_FAIL;

	Engine::CComponent*			pComponent = NULL;

	// Renderer
	pComponent = Engine::CRenderer::Create(m_pDevice);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Renderer", pComponent);

	// Transform
	pComponent = Engine::CTransform::Create(m_pDevice);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Transform", pComponent);

	// RcTex
	pComponent = Engine::CRcTex::Create(m_pDevice);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Buffer_RcTex", pComponent);

	// CubeCol
	pComponent = Engine::CCubeCol::Create(m_pDevice);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Buffer_CubeCol", pComponent);

	// CubeTex
	pComponent = Engine::CCubeTex::Create(m_pDevice);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Buffer_CubeTex", pComponent);

	// Material
	pComponent = Engine::CMaterial::Create(m_pDevice, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f), D3DXCOLOR(1.f, 1.f, 1.f, 1.f), D3DXCOLOR(0.f, 0.f, 0.f, 0.f), 50.f);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Material", pComponent);

	// CollisionBox
	pComponent = Engine::CCollisionBox::Create(m_pDevice);
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_CollisionBox", pComponent);
	
	// Shader Default
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_Default.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Shader_Default", pComponent);

	// Shader Logo
	pComponent = Engine::CShader::Create(m_pDevice, L"../Code/Shader_Logo.hpp");
	if (pComponent == NULL)
		return E_FAIL;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Shader_Logo", pComponent);

	return S_OK;
}

void CMainApp::Free(void)
{
	Engine::Safe_Release(m_pDevice);

	Engine::Release_Utility();

	Engine::Release_System();
}

CMainApp* CMainApp::Create(void)
{
	CMainApp* pMainApp = new CMainApp();

	if (FAILED(pMainApp->Initialize()))
	{
		MessageBox(NULL, L"CMainApp Create Failed", L"Error", MB_OK);
		::Safe_Release(pMainApp);
	}

	return pMainApp;
}
