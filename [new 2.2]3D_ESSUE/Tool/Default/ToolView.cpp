
// ToolView.cpp : CToolView Ŭ������ ����
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
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CToolView::OnFilePrintPreview)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CToolView ����/�Ҹ�

HWND g_hWnd;

CToolView::CToolView()
: m_pDevice(NULL)
, m_pCamera(NULL)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

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
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CToolView �׸���

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
	Engine::Get_GraphicDev()->Render_Begin();

	m_pBack->Render();

	Engine::Get_GraphicDev()->Render_End();
}


// CToolView �μ�


void CToolView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
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


// CToolView ����

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView �޽��� ó����

void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CToolView::Ready_Setting(void)
{
	//// ����̽�
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CView::OnMouseMove(nFlags, point);

	Engine::Get_InputDevice()->Update();

	((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.UpdateData(TRUE);

	m_pCamera->Update(1, 0.1f);

	((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.UpdateData(FALSE);

	Invalidate(FALSE);
}

void CToolView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	m_pCamera->Update(0, 0.1f);

	Invalidate(FALSE);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);

}
