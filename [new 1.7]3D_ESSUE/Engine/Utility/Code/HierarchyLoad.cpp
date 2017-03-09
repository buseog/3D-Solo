#include "HierarchyLoad.h"

USING(Engine)

Engine::CHierarchyLoad::CHierarchyLoad(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
, m_dwRefCnt(0)
{
	m_pDevice->AddRef();
}

Engine::CHierarchyLoad::~CHierarchyLoad(void)
{

}

HRESULT Engine::CHierarchyLoad::Initialize(const _tchar* FilePath)
{
	m_pFilePath = FilePath;
	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoad::CreateFrame(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
	D3DXFRAME_DERIVED* pFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pFrame, sizeof(D3DXFRAME_DERIVED));

	if (Name != NULL)
		AllocateName(&pFrame->Name, Name);

	D3DXMatrixIdentity(&pFrame->TransformationMatrix);
	D3DXMatrixIdentity(&pFrame->CombineTransformationMatrix);

	*ppNewFrame = pFrame;

	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoad::CreateMeshContainer(THIS_ LPCSTR Name, 
														 CONST D3DXMESHDATA *pMeshData, 
														 CONST D3DXMATERIAL *pMaterials, 
														 CONST D3DXEFFECTINSTANCE *pEffectInstances, 
														 DWORD NumMaterials, 
														 CONST DWORD *pAdjacency, 
														 LPD3DXSKININFO pSkinInfo, 
														 LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	if (Name != NULL)
		AllocateName(&pMeshContainer->Name, Name);

	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	LPD3DXMESH pMesh = pMeshData->pMesh;
	pMesh->AddRef();

	_ulong dwMeshFVF = pMesh->GetFVF();

	_ulong dwTriCnt = pMesh->GetNumFaces();

	pMeshContainer->pAdjacency = new _ulong[dwTriCnt * 3];

	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwTriCnt * 3);

	if (dwMeshFVF & D3DFVF_NORMAL)
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwMeshFVF, m_pDevice, &pMeshContainer->MeshData.pMesh);

		Engine::Safe_Release(pMesh);
	}
	else
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwMeshFVF | D3DFVF_NORMAL, m_pDevice, &pMeshContainer->MeshData.pMesh);

		D3DXComputeNormals(pMeshContainer->MeshData.pMesh, pMeshContainer->pAdjacency);

		Engine::Safe_Release(pMesh);
	}

	pMeshContainer->NumMaterials = NumMaterials == 0 ? 1 : NumMaterials;

	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->pTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];

	if (NumMaterials != 0)
	{
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * pMeshContainer->NumMaterials);

		for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
		{
			_tchar szFullPath[128] = L"";
			_tchar szFileName[128] = L"";

			MultiByteToWideChar(CP_ACP, 0, pMeshContainer->pMaterials[i].pTextureFilename, strlen(pMeshContainer->pMaterials[i].pTextureFilename), szFileName, 128);

			lstrcpy(szFullPath, m_pFilePath);
			lstrcat(szFullPath, szFileName);

			if (FAILED(D3DXCreateTextureFromFile(m_pDevice, szFullPath, &pMeshContainer->pTextures[i])))
				return E_FAIL;
		}
	}
	else
	{
		pMeshContainer->pMaterials[0].MatD3D.Diffuse = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pMeshContainer->pMaterials[0].MatD3D.Specular = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);	
		pMeshContainer->pMaterials[0].MatD3D.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pMeshContainer->pMaterials[0].MatD3D.Ambient = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pMeshContainer->pMaterials[0].MatD3D.Power = 1.0f;

		pMeshContainer->pTextures[0] = NULL;
	}

	if (pSkinInfo != NULL)
	{
		pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), pMeshContainer->MeshData.pMesh->GetFVF(), m_pDevice, &pMeshContainer->pOriMesh);

		pMeshContainer->pSkinInfo = pSkinInfo;
		pMeshContainer->pSkinInfo->AddRef();

		pMeshContainer->dwBoneCnt = pSkinInfo->GetNumBones();

		pMeshContainer->ppCombineTransformationMatrices = new MyMat*[pMeshContainer->dwBoneCnt];
		pMeshContainer->pOffsetMatrices = new MyMat[pMeshContainer->dwBoneCnt];
		pMeshContainer->pRenderingMatrices = new MyMat[pMeshContainer->dwBoneCnt];

		for (_ulong i = 0; i < pMeshContainer->dwBoneCnt; ++i)
			pMeshContainer->pOffsetMatrices[i] = *pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);
	}

	*ppNewMeshContainer = pMeshContainer;

	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoad::DestroyFrame(THIS_ LPD3DXFRAME pFrameToFree)
{
	if (pFrameToFree == NULL)
		return S_OK;

	Engine::Safe_DeleteArray(pFrameToFree->Name);
	
	if (pFrameToFree->pMeshContainer != NULL)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	if (pFrameToFree->pFrameSibling != NULL)
		DestroyFrame(pFrameToFree->pFrameSibling);


	if (pFrameToFree->pFrameFirstChild != NULL)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	Engine::Safe_Delete(pFrameToFree);

	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoad::DestroyMeshContainer(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		Engine::Safe_Release(pMeshContainer->pTextures[i]);
	}
	Engine::Safe_DeleteArray(pMeshContainer->pTextures);

	Engine::Safe_Release(pMeshContainer->pOriMesh);
	
	Engine::Safe_DeleteArray(pMeshContainer->ppCombineTransformationMatrices);
	Engine::Safe_DeleteArray(pMeshContainer->pOffsetMatrices);
	Engine::Safe_DeleteArray(pMeshContainer->pRenderingMatrices);

	Engine::Safe_DeleteArray(pMeshContainer->Name);
	Engine::Safe_DeleteArray(pMeshContainer->pMaterials);
	Engine::Safe_Release(pMeshContainer->MeshData.pMesh);
	Engine::Safe_DeleteArray(pMeshContainer->pAdjacency);
	Engine::Safe_Release(pMeshContainer->pSkinInfo);
	
	Engine::Safe_Delete(pMeshContainer);

	return S_OK;
}



void Engine::CHierarchyLoad::AllocateName( char** ppSrcName, const char* pDestName )
{
	_uint iLength = strlen(pDestName);

	*ppSrcName = new char[iLength + 1];

	strcpy_s(*ppSrcName, iLength + 1, pDestName);
}

_ulong Engine::CHierarchyLoad::Release(void)
{
	if (m_dwRefCnt == 0)
	{
		Engine::Safe_Release(m_pDevice);

		delete this;

		return 0;
	}

	return m_dwRefCnt--;
}

CHierarchyLoad* Engine::CHierarchyLoad::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* Filepath)
{
	CHierarchyLoad* pHierarch = new CHierarchyLoad(pDevice);

	if (FAILED(pHierarch->Initialize(Filepath)))
	{
		MSG_BOX("HierarchLoad Created Failed");
		Engine::Safe_Release(pHierarch);
	}

	return pHierarch;
}