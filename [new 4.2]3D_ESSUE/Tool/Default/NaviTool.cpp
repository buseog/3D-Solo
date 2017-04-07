// NaviTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "NaviTool.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CNaviTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNaviTool, CDialog)

CNaviTool::CNaviTool(CWnd* pParent /*=NULL*/)
	: CDialog(CNaviTool::IDD, pParent)
	, MouseX(0)
	, MouseY(0)
	, MouseZ(0)
	, PosX(0)
	, PosY(0)
	, PosZ(0)
	, iSelCnt(0)
{
	ZeroMemory(vSelCube, sizeof(MyVec3) * 3);
}

CNaviTool::~CNaviTool()
{
}

void CNaviTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, TriggerType);
	DDX_Radio(pDX, IDC_RADIO6, NotifyType);

	DDX_Text(pDX, IDC_EDIT2, MouseX);
	DDX_Text(pDX, IDC_EDIT3, MouseY);
	DDX_Text(pDX, IDC_EDIT4, MouseZ);

	DDX_Text(pDX, IDC_EDIT5, PosX);
	DDX_Text(pDX, IDC_EDIT6, PosY);
	DDX_Text(pDX, IDC_EDIT7, PosZ);

	DDX_Control(pDX, IDC_LIST1, NaviList);
}


BEGIN_MESSAGE_MAP(CNaviTool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CNaviTool::OnDelete)
	ON_BN_CLICKED(IDC_BUTTON5, &CNaviTool::OnSave)
	ON_BN_CLICKED(IDC_BUTTON6, &CNaviTool::OnLoad)
	ON_LBN_SELCHANGE(IDC_LIST1, &CNaviTool::OnNaviList)
END_MESSAGE_MAP()

void CNaviTool::SetMousePos( POINT pt )
{
	MyMat matView, matProj;
	LPDIRECT3DDEVICE9 pDevice = ((CMainFrame*)AfxGetMainWnd())->m_pMainView->m_pDevice;

	if (pDevice == NULL)
		return;

	pDevice->GetTransform(D3DTS_VIEW, &matView);
	pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DXMatrixInverse(&matView, NULL, &matView);
	D3DXMatrixInverse(&matProj, NULL, &matProj);

	MyVec3 vMouse;
	vMouse.x = (_float)pt.x;
	vMouse.y = (_float)pt.y;
	vMouse.z = 0.f;

	D3DXVec3TransformCoord(&vMouse, &vMouse, &(matProj * matView));
	MouseX = vMouse.x;
	MouseY = vMouse.y;
}
// CNaviTool 메시지 처리기입니다.

void CNaviTool::OnDelete()
{
	vector<Engine::CNaviMesh*>::iterator iter = Engine::Get_NaviMgr()->GetNavi()->begin();
	vector<Engine::CNaviMesh*>::iterator iter_end = Engine::Get_NaviMgr()->GetNavi()->end();

	for (_int i = 0; i < SelNaviIndex; ++i)
		++iter;

	if (iter == iter_end)
		return;

	Engine::Safe_Release(*iter);
	Engine::Get_NaviMgr()->GetNavi()->erase(iter);
	NaviList.DeleteString(SelNaviIndex);	
}

void CNaviTool::OnSave()
{
	UpdateData(TRUE);

	CFileDialog	Dlg(FALSE,	
		L"dat", 
		L"*.dat",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		L"*.dat",
		this);

	if(Dlg.DoModal() == IDCANCEL)
		return;

	TCHAR		szDirPath[MAX_PATH] = L"";

	GetCurrentDirectory(sizeof(szDirPath), szDirPath);
	SetCurrentDirectory(szDirPath);
	PathRemoveFileSpec(szDirPath);

	lstrcat(szDirPath, L"\\Data");

	Dlg.m_ofn.lpstrInitialDir = szDirPath;	

	DWORD	dwByte = 0;

	HANDLE	hFile  = CreateFile(Dlg.GetPathName(), 
		GENERIC_WRITE, 
		0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);

	vector<Engine::CNaviMesh*>::iterator iter = Engine::Get_NaviMgr()->GetNavi()->begin();
	vector<Engine::CNaviMesh*>::iterator iter_end = Engine::Get_NaviMgr()->GetNavi()->end();

	SaveNavi tSave;

	for ( ; iter != iter_end; ++iter)
	{
		ZeroMemory(&tSave, sizeof(SaveNavi));

		tSave.vPointA = *(*iter)->GetPoint(Engine::CNaviMesh::POINT_A);
		tSave.vPointB = *(*iter)->GetPoint(Engine::CNaviMesh::POINT_B);
		tSave.vPointC = *(*iter)->GetPoint(Engine::CNaviMesh::POINT_C);


		WriteFile(hFile, &tSave, sizeof(SaveNavi), &dwByte, NULL);
	}
	CloseHandle(hFile);

	UpdateData(FALSE);
}

void CNaviTool::OnLoad()
{
	UpdateData(TRUE);

	CFileDialog		Dlg(TRUE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this);

	if(Dlg.DoModal() == IDCANCEL)
		return;

	TCHAR		szDirPath[MAX_PATH] = L"";

	GetCurrentDirectory(sizeof(szDirPath), szDirPath);
	SetCurrentDirectory(szDirPath);
	PathRemoveFileSpec(szDirPath);
	lstrcat(szDirPath, L"\\Data");

	Dlg.m_ofn.lpstrInitialDir = szDirPath;	

	DWORD	dwByte = 0;

	HANDLE	hFile  = CreateFile(Dlg.GetPathName(), 
		GENERIC_READ, 
		0, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);

	SaveNavi tLoad;
	LPDIRECT3DDEVICE9 pDevice = ((CMainFrame*)AfxGetMainWnd())->m_pMainView->m_pDevice;

	_ulong iIndex = 0;

	while (true)
	{
		ReadFile(hFile, &tLoad, sizeof(SaveNavi), &dwByte, NULL);

		if (dwByte == 0)
			break;

		Engine::Get_NaviMgr()->AddNaviMesh((Engine::MyVec3*)&tLoad.vPointA, (Engine::MyVec3*)&tLoad.vPointB, (Engine::MyVec3*)&tLoad.vPointC);
		_tchar szBuf[128] = L"";
		wsprintf(szBuf, L"%d", iIndex);
		NaviList.AddString(szBuf);
		++iIndex;
	}
	CloseHandle(hFile);
	UpdateData(FALSE);
}

void CNaviTool::LinkCell( const MyVec3* pPos )
{
	vSelCube[iSelCnt] = *pPos;
	++iSelCnt;

	if (iSelCnt >= 3)
	{
		Engine::Get_NaviMgr()->AddNaviMesh((Engine::MyVec3*)&vSelCube[0], (Engine::MyVec3*)&vSelCube[1], (Engine::MyVec3*)&vSelCube[2]);
		ZeroMemory(vSelCube, sizeof(MyVec3) * 3);

		iSelCnt = 0;
	}
}

void CNaviTool::OnNaviList()
{
	SelNaviIndex = NaviList.GetCurSel();

	Engine::Get_NaviMgr()->Picking(SelNaviIndex, true);
}

BOOL CNaviTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
