
// ToolView.cpp : CToolView 클래스의 구현
//

#include "stdafx.h"
#include "Tool.h"

#include "ToolDoc.h"
#include "ToolView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CToolView::OnFilePrintPreview)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CToolView 생성/소멸

HWND g_hWnd;

CToolView::CToolView()
: m_pDevice(NULL)
, m_pCamera(NULL)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CToolView::~CToolView()
{
	Engine::Safe_Release(m_pDevice);
	Engine::Safe_Release(m_pBack);
	Engine::Safe_Release(m_pCamera);

	Engine::Release_Utility();
	Engine::Release_System();
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CToolView 그리기

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	Engine::Get_GraphicDev()->Render_Begin();

	m_pBack->Render();

	Engine::Get_GraphicDev()->Render_End();
}


// CToolView 인쇄


void CToolView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CToolView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CToolView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CToolView 진단

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView 메시지 처리기

void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	g_hWnd = m_hWnd;

	Ready_Setting();

	CMainFrame*		pMainFrm = ((CMainFrame*)AfxGetMainWnd());

	RECT		rcWindow;
	pMainFrm->GetWindowRect(&rcWindow);


	SetRect(&rcWindow, 0, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top);

	RECT		rcMainView;
	GetClientRect(&rcMainView);

	float		fRowFrm = float(rcWindow.right - rcMainView.right);
	float		fColFrm = float(rcWindow.bottom - rcMainView.bottom);

	pMainFrm->SetWindowPos(NULL, 50, 0, int(WINCX + fRowFrm), int(WINCY + fColFrm), SWP_NOZORDER);

	m_pBack = CBackGround::Create(m_pDevice);
	m_pCamera = CDynamicCamera::Create(m_pDevice, &MyVec3(0.f, 100.f, -100.f), &MyVec3(0.f, 0.f, 0.f), &MyVec3(0.f, 1.f, 0.f));
}

void CToolView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CToolView::Ready_Setting(void)
{
	//// 디바이스
	if (Engine::Get_GraphicDev()->InitGraphicDev(g_hWnd, Engine::CGraphicDev::MODE_WIN, WINCX, WINCY))
		return;

	m_pDevice = Engine::Get_GraphicDev()->GetDevice();
	m_pDevice->AddRef();

	//// Device Input
	if (Engine::Get_InputDevice()->Initialize(AfxGetInstanceHandle(), g_hWnd))
		return;

	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//// FontMgr
	if (Engine::Get_FontMgr()->AddFont(m_pDevice, L"Font_Batang", L"Batang", 20, 20, FW_HEAVY))
		return;

	Ready_Component();
}

void CToolView::Ready_Component(void)
{
	if (Engine::Get_ComponentMgr()->ReserveComponentMgr(SCENE_END))
		return;

	Engine::CComponent*			pComponent = NULL;

	// Transform
	pComponent = Engine::CTransform::Create(m_pDevice);
	if (pComponent == NULL)
		return;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Transform", pComponent);

	// RcTex
	pComponent = Engine::CRcTex::Create(m_pDevice);
	if (pComponent == NULL)
		return;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Buffer_RcTex", pComponent);

	// CubeCol
	pComponent = Engine::CCubeCol::Create(m_pDevice);
	if (pComponent == NULL)
		return;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Buffer_CubeCol", pComponent);

	//// Mesh =============
	// BackGround
	pComponent = Engine::CStaticMesh::Create(m_pDevice, L"../../Client/Bin/Resource/Mesh/StaticMesh/Map/BossMap/", L"BossMap.X");
	if (pComponent == NULL)
		return;
	Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, L"Component_Mesh_BossMap", pComponent);

}


void CToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CView::OnMouseMove(nFlags, point);

	Engine::Get_InputDevice()->Update();

	((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.UpdateData(TRUE);

	m_pCamera->Update(1, 0.1f);

	((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.UpdateData(FALSE);

	Invalidate(FALSE);
}

void CToolView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_pCamera->Update(0, 0.1f);

	Invalidate(FALSE);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);

}
