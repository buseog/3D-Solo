#include "DynamicMesh.h"
#include "HierarchyLoad.h"

USING(Engine)

Engine::CDynamicMesh::CDynamicMesh(LPDIRECT3DDEVICE9 pDevice)
: CMesh(pDevice)
, m_pLoader(NULL)
, m_pAniController(NULL)
, m_pRootFrame(NULL)
{

}

Engine::CDynamicMesh::CDynamicMesh(const CDynamicMesh& rhs)
: CMesh(rhs)
, m_pLoader(rhs.m_pLoader)
, m_pAniController(CAnimationControl::Create(*rhs.m_pAniController))
, m_pRootFrame(rhs.m_pRootFrame)
, m_MeshContainerList(rhs.m_MeshContainerList)
, m_matParent(rhs.m_matParent)
{
	m_pLoader->AddRef();
}

Engine::CDynamicMesh::~CDynamicMesh(void)
{

}

HRESULT Engine::CDynamicMesh::Initialize(const _tchar* FilePath, const _tchar* FileName)
{
	_tchar	szFullPath[128] = L"";

	lstrcpy(szFullPath, FilePath);
	lstrcat(szFullPath, FileName);

	m_pLoader = CHierarchyLoad::Create(m_pDevice, FilePath);
	if (m_pLoader == NULL)
		return E_FAIL;

	 m_pAniController = CAnimationControl::Create(m_pDevice);
	 if (m_pAniController == NULL)
		 return E_FAIL;

	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath, D3DXMESH_MANAGED, m_pDevice, m_pLoader, NULL, &m_pRootFrame, m_pAniController->GetAnimationController())))
		return E_FAIL;

	D3DXMatrixIdentity(&m_matParent);

	UpdateCombineMatrix((D3DXFRAME_DERIVED*)m_pRootFrame, &m_matParent);

	SetUpMatrixAdress((D3DXFRAME_DERIVED*)m_pRootFrame);

	return S_OK;
}

void Engine::CDynamicMesh::Render(void)
{
	MESHCONTAINERLIST::iterator iter = m_MeshContainerList.begin();
	MESHCONTAINERLIST::iterator iter_end = m_MeshContainerList.end();

	for ( ; iter != iter_end; ++iter)
	{
		for (_ulong i = 0; i < (*iter)->dwBoneCnt; ++i)
		{
			(*iter)->pRenderingMatrices[i] = (*iter)->pOffsetMatrices[i] * *(*iter)->ppCombineTransformationMatrices[i];
		}

		void*	pSrcVertices = NULL;
		void*	pDestVertices = NULL;

		(*iter)->pOriMesh->LockVertexBuffer(0, &pSrcVertices);
		(*iter)->MeshData.pMesh->LockVertexBuffer(0, &pDestVertices);

		(*iter)->pSkinInfo->UpdateSkinnedMesh((*iter)->pRenderingMatrices, NULL, pSrcVertices, pDestVertices);

		(*iter)->pOriMesh->UnlockVertexBuffer();
		(*iter)->MeshData.pMesh->UnlockVertexBuffer();

		for (_ulong i = 0; i < (*iter)->NumMaterials; ++i)
		{
			m_pDevice->SetTexture(0, (*iter)->pTextures[i]);
			(*iter)->MeshData.pMesh->DrawSubset(i);
		}
	}
}

void Engine::CDynamicMesh::UpdateCombineMatrix( D3DXFRAME_DERIVED* pFrame, const MyMat* pMatRix )
{
	pFrame->CombineTransformationMatrix = pFrame->TransformationMatrix * (*pMatRix);

	if (pFrame->pFrameSibling != NULL)
		UpdateCombineMatrix((D3DXFRAME_DERIVED*)pFrame->pFrameSibling, pMatRix);

	if (pFrame->pFrameFirstChild != NULL)
		UpdateCombineMatrix((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild, &pFrame->CombineTransformationMatrix);
}

void Engine::CDynamicMesh::SetUpMatrixAdress( D3DXFRAME_DERIVED* pRoot )
{
	if (pRoot->pMeshContainer != NULL)
	{
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pRoot->pMeshContainer;

		for (_ulong i = 0; i < pMeshContainer->dwBoneCnt; ++i)
		{
			const char* pFrameName = pMeshContainer->pSkinInfo->GetBoneName(i);

			D3DXFRAME_DERIVED* pFrameFind = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pFrameName);

			pMeshContainer->ppCombineTransformationMatrices[i] = &pFrameFind->CombineTransformationMatrix;
		}

		m_MeshContainerList.push_back(pMeshContainer);
	}

	if (pRoot->pFrameSibling != NULL)
		SetUpMatrixAdress((D3DXFRAME_DERIVED*)pRoot->pFrameSibling);

	if (pRoot->pFrameFirstChild != NULL)
		SetUpMatrixAdress((D3DXFRAME_DERIVED*)pRoot->pFrameFirstChild);
}

void Engine::CDynamicMesh::SetAnimation( const _uint& iIndex )
{
	m_pAniController->SetAnimation(iIndex);
}

void Engine::CDynamicMesh::PlayAnimation( const _float& fDelta )
{
	m_pAniController->PlayAnimation(fDelta);

	UpdateCombineMatrix((D3DXFRAME_DERIVED*)m_pRootFrame, &m_matParent);
}

_bool Engine::CDynamicMesh::EndAnimation( void )
{
	return m_pAniController->EndAnimation();
}

void Engine::CDynamicMesh::ComputeMesh( const _uint& iIndex, MyVec3* pMin, MyVec3* pMax )
{
	D3DVERTEXELEMENT9	Element[MAX_FVF_DECL_SIZE];
	ZeroMemory(Element, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);

	MESHCONTAINERLIST::iterator iter = m_MeshContainerList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	if (iter == m_MeshContainerList.end())
		return;

	(*iter)->MeshData.pMesh->GetDeclaration(Element);

	_ushort byOffset = 0;

	for (_ulong i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (Element[i].Usage == D3DDECLUSAGE_POSITION)
		{
			byOffset = Element[i].Offset;
			break;
		}
	}

	_ushort*	pVertex = NULL;

	(*iter)->MeshData.pMesh->LockVertexBuffer(0, (void**)&pVertex);

	D3DXComputeBoundingBox((MyVec3*)(pVertex + byOffset), (*iter)->MeshData.pMesh->GetNumVertices(), D3DXGetFVFVertexSize((*iter)->MeshData.pMesh->GetFVF()), pMin, pMax);

	(*iter)->MeshData.pMesh->UnlockVertexBuffer();
}

void Engine::CDynamicMesh::Free(void)
{
	Engine::CMesh::Free();

	if (m_bClone == false)
		m_pLoader->DestroyFrame(m_pRootFrame);

	m_MeshContainerList.clear();

	Engine::Safe_Release(m_pAniController);

	Engine::Safe_Release(m_pLoader);
}

CComponent* Engine::CDynamicMesh::Clone(void)
{
	return new CDynamicMesh(*this);
}

CDynamicMesh* Engine::CDynamicMesh::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* FilePath, const _tchar* FileName)
{
	CDynamicMesh* pDynamic = new CDynamicMesh(pDevice);

	if (FAILED(pDynamic->Initialize(FilePath, FileName)))
	{
		MSG_BOX("DynamicMesh Created Failed");
		Engine::Safe_Release(pDynamic);
	}

	return pDynamic;
}

D3DXMATRIX* Engine::CDynamicMesh::GetPartsMatrix( const char* PartsKey )
{
	D3DXFRAME_DERIVED* pFrameFind = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, PartsKey);

	if (pFrameFind == NULL)
		return NULL;

	return &pFrameFind->CombineTransformationMatrix;
}
