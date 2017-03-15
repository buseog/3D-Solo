#include "HierarchyLoader.h"

USING(Engine)

Engine::CHierarchyLoader::CHierarchyLoader(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath)
: m_pGraphicDev(pGraphicDev)
, m_pFilePath(pFilePath)
, m_dwRefCnt(0)
{
	m_pGraphicDev->AddRef();
}

Engine::CHierarchyLoader::~CHierarchyLoader(void)
{

}

HRESULT Engine::CHierarchyLoader::Ready_HierarchyLoader(void)
{
	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoader::CreateFrame(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
	D3DXFRAME_DERIVED*			pNewFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pNewFrame, sizeof(D3DXFRAME_DERIVED));

	if(NULL != Name)
		Allocate_Name(&pNewFrame->Name, Name);

	pNewFrame->CombineTransformationMatrix = *D3DXMatrixIdentity(&pNewFrame->TransformationMatrix);

	*ppNewFrame = pNewFrame;

	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoader::CreateMeshContainer(THIS_ LPCSTR Name
														   , CONST D3DXMESHDATA *pMeshData
														   , CONST D3DXMATERIAL *pMaterials
														   , CONST D3DXEFFECTINSTANCE *pEffectInstances
														   , DWORD NumMaterials
														   , CONST DWORD *pAdjacency
														   , LPD3DXSKININFO pSkinInfo
														   , LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
	D3DXMESHCONTAINER_DERIVED*			pNewMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pNewMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	if(NULL != Name)
		Allocate_Name(&pNewMeshContainer->Name, Name);

	pNewMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	LPD3DXMESH			pMesh = pMeshData->pMesh;
	pMesh->AddRef();

	_ulong dwMeshFVF = pMesh->GetFVF();

	// Æú¸®°ï °¹¼ö¸¦ ¾ò¾î¿Â´Ù.
	_ulong dwNumFaces = pMesh->GetNumFaces();
		
	pNewMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3];

	memcpy(pNewMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumFaces * 3);

	if(!(dwMeshFVF & D3DFVF_NORMAL))
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwMeshFVF | D3DFVF_NORMAL, m_pGraphicDev, &pNewMeshContainer->MeshData.pMesh);

		D3DXComputeNormals(pNewMeshContainer->MeshData.pMesh, pNewMeshContainer->pAdjacency);

		Engine::Safe_Release(pMesh);
	}
	else
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwMeshFVF, m_pGraphicDev, &pNewMeshContainer->MeshData.pMesh);

		Engine::Safe_Release(pMesh);
	}

	pNewMeshContainer->NumMaterials = NumMaterials == 0 ? 1 : NumMaterials;

	pNewMeshContainer->pMaterials = new D3DXMATERIAL[pNewMeshContainer->NumMaterials];
	pNewMeshContainer->pTextures = new LPDIRECT3DTEXTURE9[pNewMeshContainer->NumMaterials];

	if(0 != NumMaterials)
	{
		memcpy(pNewMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * pNewMeshContainer->NumMaterials);

		for (_ulong i = 0; i < pNewMeshContainer->NumMaterials; ++i)
		{
			_tchar			szFileName[128] = L"";
			_tchar			szFullPath[128] = L"";

			MultiByteToWideChar(CP_ACP, 0, pNewMeshContainer->pMaterials[i].pTextureFilename, strlen(pNewMeshContainer->pMaterials[i].pTextureFilename)
				, szFileName, 128);

			lstrcpy(szFullPath, m_pFilePath);
			lstrcat(szFullPath, szFileName);

			if(FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->pTextures[i])))
				return E_FAIL;
		}
	}
	else
	{
		pNewMeshContainer->pMaterials[0].MatD3D.Diffuse = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Specular = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Ambient = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Power = 1.0f;

		pNewMeshContainer->pTextures[0] = NULL;			
	}

	if(NULL != pSkinInfo)
	{
		pNewMeshContainer->MeshData.pMesh->CloneMeshFVF(pNewMeshContainer->MeshData.pMesh->GetOptions(), pNewMeshContainer->MeshData.pMesh->GetFVF(), m_pGraphicDev, &pNewMeshContainer->pOriMesh);

		pNewMeshContainer->pSkinInfo = pSkinInfo;
		pNewMeshContainer->pSkinInfo->AddRef();

		pNewMeshContainer->dwBoneCnt = pSkinInfo->GetNumBones();

		pNewMeshContainer->ppCombineTransformationMatrices = new MyMat*[pNewMeshContainer->dwBoneCnt];
		pNewMeshContainer->pOffsetMatrices = new MyMat[pNewMeshContainer->dwBoneCnt];
		pNewMeshContainer->pRenderingMatrices = new MyMat[pNewMeshContainer->dwBoneCnt];

		for (_ulong i = 0; i < pNewMeshContainer->dwBoneCnt; ++i)		
			pNewMeshContainer->pOffsetMatrices[i] = *pNewMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);		
	}
	

	*ppNewMeshContainer = pNewMeshContainer;		
	
	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoader::DestroyFrame(THIS_ LPD3DXFRAME pFrameToFree)
{
	if(NULL == pFrameToFree)
		return S_OK;

	Engine::Safe_DeleteArray(pFrameToFree->Name);

	if(NULL != pFrameToFree->pMeshContainer)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	if(NULL != pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	if(NULL != pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	Engine::Safe_Delete(pFrameToFree);

	return S_OK;
}

STDMETHODIMP Engine::CHierarchyLoader::DestroyMeshContainer(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED*			pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		Engine::Safe_Release(pMeshContainer->pTextures[i]);
	}
	Engine::Safe_DeleteArray(pMeshContainer->pTextures);

	Engine::Safe_Release(pMeshContainer->pOriMesh);

	Engine::Safe_DeleteArray(pMeshContainer->pOffsetMatrices);
	Engine::Safe_DeleteArray(pMeshContainer->ppCombineTransformationMatrices);
	Engine::Safe_DeleteArray(pMeshContainer->pRenderingMatrices);

	Engine::Safe_DeleteArray(pMeshContainer->Name);
	Engine::Safe_DeleteArray(pMeshContainer->pMaterials);
	Engine::Safe_Release(pMeshContainer->MeshData.pMesh);
	Engine::Safe_DeleteArray(pMeshContainer->pAdjacency);
	Engine::Safe_Release(pMeshContainer->pSkinInfo);

	Engine::Safe_Delete(pMeshContainer);

	return S_OK;
}

void Engine::CHierarchyLoader::Allocate_Name(char** ppSrcName, const char* pDestName)
{
	_uint		iLength = strlen(pDestName);

	*ppSrcName = new char[iLength + 1];

	strcpy(*ppSrcName, pDestName);		
}

CHierarchyLoader* Engine::CHierarchyLoader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath)
{
	CHierarchyLoader*		pInstance = new CHierarchyLoader(pGraphicDev, pFilePath);

	if(FAILED(pInstance->Ready_HierarchyLoader()))
	{
		MSG_BOX("CHierarchyLoader Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong Engine::CHierarchyLoader::Release(void)
{
	if(0 == m_dwRefCnt)
	{
		Engine::Safe_Release(m_pGraphicDev);		

		delete this;

		return 0;
	}

	return m_dwRefCnt--;	
}

