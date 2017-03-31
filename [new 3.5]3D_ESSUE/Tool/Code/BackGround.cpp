#include "StdAfx.h"
#include "BackGround.h"
#include "..\Default\MainFrm.h"
#include "Obstacle.h"
#include "CubeObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBackGround::CBackGround( LPDIRECT3DDEVICE9 pDevice )
: CGameObject(pDevice)
, m_pTransformCom(NULL)
, m_pMeshCom(NULL)
, m_pPick(NULL)
, m_pPickCube(NULL)

{

}

CBackGround::~CBackGround( void )
{

}

HRESULT CBackGround::Initialize( void )
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &Engine::MyVec3(0.1f, 0.1f, 0.1f));
	m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &Engine::MyVec3(0.f, D3DX_PI, 0.f));
	m_pTransformCom->Update();

	return S_OK;
}

_int CBackGround::Update( const _float& fDelta )
{
	Engine::CGameObject::Update(fDelta);

	return 0;
}

void CBackGround::Render( void )
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pTransformCom->SetUpDevice();
	m_pMeshCom->Render();
	vector<CObstacle*>::iterator iter = m_vecObstacle.begin();
	vector<CObstacle*>::iterator iter_end = m_vecObstacle.end();

	for ( ; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}

	vector<CCubeObj*>::iterator Cubeiter = m_vecCube.begin();
	vector<CCubeObj*>::iterator Cubeiter_end = m_vecCube.end();

	for ( ; Cubeiter != Cubeiter_end; ++Cubeiter)
	{
		(*Cubeiter)->Render();
	}
}

HRESULT CBackGround::Ready_Component( void )
{
	m_pTransformCom = (Engine::CTransform*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Transform");
	if (m_pTransformCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_DO, L"Com_Transform", m_pTransformCom)))
		return E_FAIL;

	m_pMeshCom = (Engine::CStaticMesh*)Engine::Get_ComponentMgr()->CloneComponent(SCENE_STATIC, L"Component_Mesh_BossMap");
	if (m_pMeshCom == NULL)
		return E_FAIL;
	if (FAILED(AddComponent(Engine::CGameObject::UPDATE_NONE, L"Com_Mesh", m_pMeshCom)))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Free( void )
{
	Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pMeshCom);

	for_each(m_vecObstacle.begin(), m_vecObstacle.end(), Engine::CReleaseSingle());
	m_vecObstacle.clear();

	for_each(m_vecCube.begin(), m_vecCube.end(), Engine::CReleaseSingle());
	m_vecCube.clear();

	Engine::CGameObject::Free();
}

CBackGround* CBackGround::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CBackGround* pBackGround = new CBackGround(pDevice);

	if (FAILED(pBackGround->Initialize()))
	{
		MSG_BOX("Terrain Created Failed");
		Engine::Safe_Release(pBackGround);
	}

	return pBackGround;
}

void CBackGround::Picking(void)
{
	POINT	pt;

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	D3DXVECTOR3 vMouse;

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	vMouse.x = (float(pt.x)  / (WINCX / 2) - 1.f) / matProj._11;
	vMouse.y = (float(-pt.y) / (WINCY / 2) + 1.f) / matProj._22;
	vMouse.z = 1.f;

	D3DXVECTOR3 vRayPos, vRayDir;
	vRayPos = D3DXVECTOR3(0.f,0.f,0.f);
	vRayDir = vMouse - vRayPos;
	D3DXVec3Normalize(&vRayDir, &vRayDir);

	D3DXMATRIX matRView, matRWorld;
	D3DXMatrixInverse(&matRView, 0, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matRView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matRView);

	_int iHit = false;
	_ulong dwFaceIndex = 0;
	_float fU = 0.f;
	_float fV = 0.f;
	_float fDist = 0.f;

	if (((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.TriggerType == 0)
	{
		D3DXMatrixInverse(&matRWorld, 0, m_pTransformCom->GetWorldMatrix());
		D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matRWorld);
		D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matRWorld);

		D3DXIntersect(m_pMeshCom->GetMesh(), &vRayPos, &vRayDir, &iHit, &dwFaceIndex, &fU, &fV, &fDist, NULL, NULL);

		D3DXVec3TransformCoord(&vRayPos, &vRayPos, m_pTransformCom->GetWorldMatrix());
		D3DXVec3TransformCoord(&vRayDir, &vRayDir, m_pTransformCom->GetWorldMatrix());

		if (iHit == TRUE)
		{
			_uint iSel = ((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.SelObstacleInfo;
			MyVec3 vPos = vRayPos + (vRayDir * fDist);
			CObstacle* pObstacle = CObstacle::Create(m_pDevice, ((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.vecMeshKey[iSel], &vPos);
			m_vecObstacle.push_back(pObstacle);
			((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.AddObstacleList(pObstacle);
		}
	}
	else if (((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.TriggerType == 1)
	{
		vector<CObstacle*>::iterator iter = m_vecObstacle.begin();
		vector<CObstacle*>::iterator iter_end = m_vecObstacle.end();

		for ( ; iter != iter_end; ++iter)
		{
			vRayPos = D3DXVECTOR3(0.f,0.f,0.f);
			vRayDir = vMouse - vRayPos;
			D3DXVec3Normalize(&vRayDir, &vRayDir);

			D3DXMATRIX matRView, matRWorld;
			D3DXMatrixInverse(&matRView, 0, &matView);
			D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matRView);
			D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matRView);

			D3DXMatrixInverse(&matRWorld, 0, (*iter)->m_pTransformCom->GetWorldMatrix());
			D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matRWorld);
			D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matRWorld);

			D3DXIntersect((*iter)->m_pMeshCom->GetMesh(), &vRayPos, &vRayDir, &iHit, &dwFaceIndex, &fU, &fV, &fDist, NULL, NULL);

			D3DXVec3TransformCoord(&vRayPos, &vRayPos, (*iter)->m_pTransformCom->GetWorldMatrix());
			D3DXVec3TransformCoord(&vRayDir, &vRayDir, (*iter)->m_pTransformCom->GetWorldMatrix());

			if (iHit == TRUE)
			{
				CMapTool* pMapTool = &((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool;

				if (m_pPick != NULL)
					m_pPick->m_pCollisionCom->SetCollision(false);

				m_pPick = (*iter);

				pMapTool->ScaleX = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_SCALE)->x;
				pMapTool->ScaleY = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_SCALE)->y;
				pMapTool->ScaleZ = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_SCALE)->z;

				pMapTool->AngleX = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->x;
				pMapTool->AngleY = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->y;
				pMapTool->AngleZ = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->z;

				pMapTool->PosX = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->x;
				pMapTool->PosY = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->y;
				pMapTool->PosZ = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->z;

				m_pPick->m_pCollisionCom->SetCollision(true);

				return;
			}
		}
	}
}

void CBackGround::NaviPicking( void )
{
	POINT	pt;

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	D3DXVECTOR3 vMouse;

	MyMat matView, matProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	vMouse.x = (float(pt.x)  / (WINCX / 2) - 1.f) / matProj._11;
	vMouse.y = (float(-pt.y) / (WINCY / 2) + 1.f) / matProj._22;
	vMouse.z = 1.f;

	D3DXVECTOR3 vRayPos, vRayDir;
	vRayPos = D3DXVECTOR3(0.f,0.f,0.f);
	vRayDir = vMouse - vRayPos;
	D3DXVec3Normalize(&vRayDir, &vRayDir);

	D3DXMATRIX matRView, matRWorld;
	D3DXMatrixInverse(&matRView, 0, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matRView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matRView);

	_int iHit = false;
	_ulong dwFaceIndex = 0;
	_float fU = 0.f;
	_float fV = 0.f;
	_float fDist = 0.f;

	if (((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.TriggerType == 0)
	{
		D3DXMatrixInverse(&matRWorld, 0, m_pTransformCom->GetWorldMatrix());
		D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matRWorld);
		D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matRWorld);

		D3DXIntersect(m_pMeshCom->GetMesh(), &vRayPos, &vRayDir, &iHit, &dwFaceIndex, &fU, &fV, &fDist, NULL, NULL);

		D3DXVec3TransformCoord(&vRayPos, &vRayPos, m_pTransformCom->GetWorldMatrix());
		D3DXVec3TransformNormal(&vRayDir, &vRayDir, m_pTransformCom->GetWorldMatrix());

		if (iHit == TRUE)
		{
			MyVec3 vPos = vRayPos + (vRayDir * fDist);
			CCubeObj* pObstacle = CCubeObj::Create(m_pDevice, &vPos);
			m_vecCube.push_back(pObstacle);
		}
	}

	else if (((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.TriggerType == 1)
	{
		vector<CCubeObj*>::iterator iter = m_vecCube.begin();
		vector<CCubeObj*>::iterator iter_end = m_vecCube.end();

		for ( ; iter != iter_end; ++iter)
		{
			D3DXVECTOR3 vRayPos, vRayDir;
			vRayPos = D3DXVECTOR3(0.f,0.f,0.f);
			vRayDir = vMouse - vRayPos;
			D3DXVec3Normalize(&vRayDir, &vRayDir);

			D3DXMATRIX matRView, matRWorld;
			D3DXMatrixInverse(&matRView, 0, &matView);
			D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matRView);
			D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matRView);

			D3DXMatrixInverse(&matRWorld, 0, (*iter)->m_pTransformCom->GetWorldMatrix());
			D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matRWorld);
			D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matRWorld);

			if (D3DXIntersectTri(&MyVec3(-1.f, 1.f, 1.f), &MyVec3(1.f, 1.f, 1.f), &MyVec3(1.f, 1.f, -1.f),
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				if (m_pPickCube)
					m_pPickCube->SetColor(D3DXCOLOR(0.f, 1.f, 0.f, 1.f));
				m_pPickCube = (*iter);
				m_pPickCube->SetColor(D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

				D3DXVec3TransformCoord(&vRayPos, &vRayPos, (*iter)->m_pTransformCom->GetWorldMatrix());
				D3DXVec3TransformNormal(&vRayDir, &vRayDir, (*iter)->m_pTransformCom->GetWorldMatrix());

				MyVec3 vPos = vRayPos + (vRayDir * fDist);

				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosX = vPos.x;
				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosY = vPos.y;
				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosZ = vPos.z;
				return;
			}
			else if (D3DXIntersectTri(&MyVec3(-1.f, 1.f, 1.f), &MyVec3(1.f, 1.f, -1.f), &MyVec3(-1.f, 1.f, -1.f),
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				if (m_pPickCube)
					m_pPickCube->SetColor(D3DXCOLOR(0.f, 1.f, 0.f, 1.f));
				m_pPickCube = (*iter);
				m_pPickCube->SetColor(D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

				D3DXVec3TransformCoord(&vRayPos, &vRayPos, (*iter)->m_pTransformCom->GetWorldMatrix());
				D3DXVec3TransformNormal(&vRayDir, &vRayDir, (*iter)->m_pTransformCom->GetWorldMatrix());

				MyVec3 vPos = vRayPos + (vRayDir * fDist);

				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosX = vPos.x;
				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosY = vPos.y;
				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosZ = vPos.z;
				return;
			}
		}
	}
	else
	{
		vector<CCubeObj*>::iterator iter = m_vecCube.begin();
		vector<CCubeObj*>::iterator iter_end = m_vecCube.end();

		for ( ; iter != iter_end; ++iter)
		{
			D3DXVECTOR3 vRayPos, vRayDir;
			vRayPos = D3DXVECTOR3(0.f,0.f,0.f);
			vRayDir = vMouse - vRayPos;
			D3DXVec3Normalize(&vRayDir, &vRayDir);

			D3DXMATRIX matRView, matRWorld;
			D3DXMatrixInverse(&matRView, 0, &matView);
			D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matRView);
			D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matRView);

			D3DXMatrixInverse(&matRWorld, 0, (*iter)->m_pTransformCom->GetWorldMatrix());
			D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matRWorld);
			D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matRWorld);

	
			if (D3DXIntersectTri(&MyVec3(-1.f, 1.f, 1.f), &MyVec3(1.f, 1.f, 1.f), &MyVec3(1.f, 1.f, -1.f),
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				if (m_pPickCube)
					m_pPickCube->SetColor(D3DXCOLOR(0.f, 1.f, 0.f, 1.f));

				m_pPickCube = (*iter);
				m_pPickCube->SetColor(D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosX = m_pPickCube->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->x;
				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosY = m_pPickCube->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->y;
				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosZ = m_pPickCube->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->z;

				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.LinkCell((MyVec3*)m_pPickCube->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS));
				return;
			}
			else if (D3DXIntersectTri(&MyVec3(-1.f, 1.f, 1.f), &MyVec3(1.f, 1.f, -1.f), &MyVec3(-1.f, 1.f, -1.f),
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				if (m_pPickCube)
					m_pPickCube->SetColor(D3DXCOLOR(0.f, 1.f, 0.f, 1.f));

				m_pPickCube = (*iter);
				m_pPickCube->SetColor(D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosX = m_pPickCube->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->x;
				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosY = m_pPickCube->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->y;
				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.PosZ = m_pPickCube->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->z;

				((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.LinkCell((MyVec3*)m_pPickCube->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS));
				return;
			}
		}
	}
}

void CBackGround::KeyCheck( void )
{
	if (m_pPick == NULL && m_pPickCube == NULL)
		return;

	if (((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_iToolType == 0)
	{
		switch (((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.NotifyType)
		{
		case 0:
			if (GetAsyncKeyState('Z') & 0x8000)
			{
				m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &MyVec3(0.01f, 0.01f, 0.01f), 1);
			}
			if (GetAsyncKeyState('X') & 0x8000)
			{
				m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_SCALE, &MyVec3(-0.01f, -0.01f, -0.01f), 1);
			}
			break;

		case 1:
			switch (((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.NotifyOptionType)
			{
			case 0:
				if (GetAsyncKeyState('Z') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.1f, 0.f, 0.f), 1);
				}
				if (GetAsyncKeyState('X') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(-0.1f, 0.f, 0.f), 1);
				}
				break;

			case 1:
				if (GetAsyncKeyState('Z') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.f, 0.1f, 0.f), 1);
				}
				if (GetAsyncKeyState('X') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.0f, -0.1f, 0.f), 1);
				}
				break;

			case 2:
				if (GetAsyncKeyState('Z') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.f, 0.f, 0.1f), 1);
				}
				if (GetAsyncKeyState('X') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_ANGLE, &MyVec3(0.f, 0.f, -0.1f), 1);
				}
				break;
			}
			break;

		case 2:
			switch (((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool.NotifyOptionType)
			{
			case 0:
				if (GetAsyncKeyState('Z') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(0.3f, 0.f, 0.f), 1);
				}
				if (GetAsyncKeyState('X') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(-0.3f, 0.f, 0.f), 1);
				}
				break;

			case 1:
				if (GetAsyncKeyState('Z') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(0.f, 0.3f, 0.f), 1);
				}
				if (GetAsyncKeyState('X') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(0.0f, -0.3f, 0.f), 1);
				}
				break;

			case 2:
				if (GetAsyncKeyState('Z') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(0.f, 0.f, 0.3f), 1);
				}
				if (GetAsyncKeyState('X') & 0x8000)
				{
					m_pPick->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(0.f, 0.f, -0.3f), 1);
				}
				break;
			}
			break;
		}

		m_pPick->m_pTransformCom->Update();

		CMapTool* pMapTool = &((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_MapTool;

		pMapTool->ScaleX = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_SCALE)->x;
		pMapTool->ScaleY = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_SCALE)->y;
		pMapTool->ScaleZ = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_SCALE)->z;

		pMapTool->AngleX = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->x;
		pMapTool->AngleY = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->y;
		pMapTool->AngleZ = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_ANGLE)->z;

		pMapTool->PosX = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->x;
		pMapTool->PosY = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->y;
		pMapTool->PosZ = m_pPick->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->z;
	}
	else if (((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_iToolType == 1)
	{
		switch (((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool.NotifyType)
		{
		case 0:
			if (GetAsyncKeyState('Z') & 0x8000)
			{
				m_pPickCube->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(0.3f, 0.f, 0.f), 1);
			}
			if (GetAsyncKeyState('X') & 0x8000)
			{
				m_pPickCube->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(-0.3f, 0.f, 0.f), 1);
			}
			break;

		case 1:
			if (GetAsyncKeyState('Z') & 0x8000)
			{
				m_pPickCube->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(0.f, 0.3f, 0.f), 1);
			}
			if (GetAsyncKeyState('X') & 0x8000)
			{
				m_pPickCube->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(0.0f, -0.3f, 0.f), 1);
			}
			break;

		case 2:
			if (GetAsyncKeyState('Z') & 0x8000)
			{
				m_pPickCube->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(0.f, 0.f, 0.3f), 1);
			}
			if (GetAsyncKeyState('X') & 0x8000)
			{
				m_pPickCube->m_pTransformCom->SetInfomation(Engine::CTransform::INFO_POS, &MyVec3(0.f, 0.f, -0.3f), 1);
			}
			break;
		}
		m_pPickCube->m_pTransformCom->Update();
		CNaviTool* pNaviTool = &((CMainFrame*)AfxGetMainWnd())->m_pMainForm->m_NaviTool;

		pNaviTool->PosX = m_pPickCube->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->x;
		pNaviTool->PosY = m_pPickCube->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->y;
		pNaviTool->PosZ = m_pPickCube->m_pTransformCom->GetInfomation(Engine::CTransform::INFO_POS)->z;
	}
}

void CBackGround::DeleteObstacle( const _uint& iIndex )
{
	vector<CObstacle*>::iterator iter = m_vecObstacle.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	Engine::Safe_Release(*iter);
	m_vecObstacle.erase(iter);
}
