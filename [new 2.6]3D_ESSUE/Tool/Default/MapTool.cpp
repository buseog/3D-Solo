// MapTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MapTool.h"
#include "MainFrm.h"
#include "Obstacle.h"
#include "BackGround.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CMapTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapTool, CDialog)

CMapTool::CMapTool(CWnd* pParent /*=NULL*/)
	: CDialog(CMapTool::IDD, pParent)
	//, TriggerType(0)
	//, NotifyType(0)
	, SelObstacleInfo(0)
	, SelObstacleList(0)
	, MouseX(0)
	, MouseY(0)
	, MouseZ(0)
	, ScaleX(0)
	, ScaleY(0)
	, ScaleZ(0)
	, AngleX(0)
	, AngleY(0)
	, AngleZ(0)
	, PosX(0)
	, PosY(0)
	, PosZ(0)
{

}

CMapTool::~CMapTool()
{
	for (_uint i = 0; i < vecMeshKey.size(); ++i)
		Engine::Safe_DeleteArray(vecMeshKey[i]);
	vecMeshKey.clear();
}

void CMapTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, TriggerType);
	DDX_Radio(pDX, IDC_RADIO3, NotifyType);
	DDX_Radio(pDX, IDC_RADIO6, NotifyOptionType);


	DDX_Text(pDX, IDC_EDIT2, MouseX);
	DDX_Text(pDX, IDC_EDIT3, MouseY);
	DDX_Text(pDX, IDC_EDIT4, MouseZ);

	DDX_Text(pDX, IDC_EDIT5, ScaleX);
	DDX_Text(pDX, IDC_EDIT6, ScaleY);
	DDX_Text(pDX, IDC_EDIT7, ScaleZ);

	DDX_Text(pDX, IDC_EDIT8, AngleX);
	DDX_Text(pDX, IDC_EDIT9, AngleY);
	DDX_Text(pDX, IDC_EDIT10, AngleZ);

	DDX_Text(pDX, IDC_EDIT11, PosX);
	DDX_Text(pDX, IDC_EDIT12, PosY);
	DDX_Text(pDX, IDC_EDIT13, PosZ);
	DDX_Control(pDX, IDC_LIST1, ObstacleInfo);
	DDX_Control(pDX, IDC_LIST2, ObstacleList);
}


BEGIN_MESSAGE_MAP(CMapTool, CDialog)

	ON_BN_CLICKED(IDC_BUTTON2, &CMapTool::OnObstacleSave)
	ON_BN_CLICKED(IDC_BUTTON3, &CMapTool::OnObstacleLoad)
	ON_LBN_SELCHANGE(IDC_LIST1, &CMapTool::OnObstacleInfo)
	ON_LBN_SELCHANGE(IDC_LIST2, &CMapTool::OnObStacleList)
	ON_BN_CLICKED(IDC_BUTTON4, &CMapTool::OnObstacleDelete)
	ON_WM_DROPFILES()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CMapTool 메시지 처리기입니다.


void CMapTool::OnObstacleSave()
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

	vector<CObstacle*>::iterator iter = ((CMainFrame*)AfxGetMainWnd())->m_pMainView->m_pBack->m_vecObstacle.begin();
	vector<CObstacle*>::iterator iter_end = ((CMainFrame*)AfxGetMainWnd())->m_pMainView->m_pBack->m_vecObstacle.end();

	SaveObstacle tSave;

	for ( ; iter != iter_end; ++iter)
	{
		ZeroMemory(&tSave, sizeof(SaveObstacle));

		lstrcpy(tSave.szKey, (*iter)->szKey);
		tSave.vScale = *(*iter)->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_SCALE);
		tSave.vAngle = *(*iter)->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE);
		tSave.vPos = *(*iter)->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS);

		WriteFile(hFile, &tSave, sizeof(SaveObstacle), &dwByte, NULL);
	}
	CloseHandle(hFile);

	UpdateData(FALSE);
}

void CMapTool::OnObstacleLoad()
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

	SaveObstacle tLoad;
	LPDIRECT3DDEVICE9 pDevice = ((CMainFrame*)AfxGetMainWnd())->m_pMainView->m_pDevice;

	while (true)
	{
		ReadFile(hFile, &tLoad, sizeof(SaveObstacle), &dwByte, NULL);

		if (dwByte == 0)
			break;

		bool bDelete = true;

		_tchar* pKey = new _tchar[lstrlen(tLoad.szKey) + 1];
		lstrcpy(pKey,tLoad.szKey);

		_tchar szFullKey[256] = L"";
		lstrcpy(szFullKey, pKey);
		lstrcat(szFullKey, L".X");

		Engine::CComponent* pFindComponent = Engine::Get_ComponentMgr()->GetComponent(SCENE_STATIC, pKey);
		if (pFindComponent == NULL)
		{
			Engine::CComponent* pComponent = Engine::CStaticMesh::Create(pDevice, L"../../Client/bin/Resource/Mesh/StaticMesh/Obstacle/", szFullKey);
			Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, pKey, pComponent);
			ObstacleInfo.AddString(pKey);
			vecMeshKey.push_back(pKey);
			bDelete = false;
		}

		CObstacle* pLoad = CObstacle::Create(pDevice, pKey, &MyVec3(0.f, 0.f, 0.f));
		pLoad->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &tLoad.vScale);
		pLoad->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &tLoad.vAngle);
		pLoad->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &tLoad.vPos);
		pLoad->m_pTransformCom->Update();

		((CMainFrame*)AfxGetMainWnd())->m_pMainView->m_pBack->m_vecObstacle.push_back(pLoad);
		ObstacleList.AddString(pKey);
		
		if (bDelete == true)
			Engine::Safe_DeleteArray(pKey);
	}
	CloseHandle(hFile);
	UpdateData(FALSE);
}

void CMapTool::OnObstacleInfo()
{
	UpdateData(TRUE);

	SelObstacleInfo = ObstacleInfo.GetCurSel();

	UpdateData(FALSE);
}

void CMapTool::OnObStacleList()
{
	UpdateData(TRUE);

	SelObstacleList = ObstacleList.GetCurSel();

	UpdateData(FALSE);
}

void CMapTool::OnObstacleDelete()
{
	UpdateData(TRUE);

	((CMainFrame*)AfxGetMainWnd())->m_pMainView->m_pBack->DeleteObstacle(SelObstacleList);

	ObstacleList.DeleteString(SelObstacleList);

	UpdateData(FALSE);
}

void CMapTool::OnDropFiles(HDROP hDropInfo)
{
	UpdateData(TRUE);

	int		iFileNumber = DragQueryFile(hDropInfo, -1, NULL, 0);

	TCHAR	szFullPath[MAX_PATH] = L"";

	for(int i = 0; i < iFileNumber; ++i)
	{
		DragQueryFile(hDropInfo, i, (LPWSTR)szFullPath, MAX_PATH);
		CFileFind finder;
		BOOL bCheck = finder.FindFile(szFullPath);
		finder.FindNextFile();
		CString cstrName = finder.GetFileTitle();

		ObstacleInfo.AddString(cstrName);

		_tchar* pKey = new _tchar[lstrlen(cstrName)+1];
		lstrcpy(pKey, cstrName);
		vecMeshKey.push_back(pKey);

		_tchar szFullKey[256] = L"";
		lstrcpy(szFullKey, cstrName);
		lstrcat(szFullKey, L".X");
	
		Engine::CComponent* pComponent = Engine::CStaticMesh::Create(((CMainFrame*)AfxGetMainWnd())->m_pMainView->m_pDevice, L"../../Client/bin/Resource/Mesh/StaticMesh/Obstacle/",szFullKey);
		Engine::Get_ComponentMgr()->AddComponent(SCENE_STATIC, pKey, pComponent);
	}

	UpdateData(FALSE);

	CDialog::OnDropFiles(hDropInfo);
}

void CMapTool::SetMousePos( POINT pt )
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

void CMapTool::AddObstacleList( CObstacle* pObstacle)
{
	ObstacleList.AddString(pObstacle->szKey);
}

