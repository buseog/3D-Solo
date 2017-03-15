#include "StaticMesh.h"

USING(Engine)

Engine::CStaticMesh::CStaticMesh(LPDIRECT3DDEVICE9 pDevice)
: Engine::CMesh(pDevice)
{

}

Engine::CStaticMesh::CStaticMesh(const CStaticMesh& rhs)
: Engine::CMesh(rhs)
, m_pAdjacency(rhs.m_pAdjacency)
, m_pSubSetBuffer(rhs.m_pSubSetBuffer)
, m_pSubSets(rhs.m_pSubSets)
, m_dwSubSetCnt(rhs.m_dwSubSetCnt)
, m_pMesh(rhs.m_pMesh)
, m_pOriMesh(rhs.m_pOriMesh)
, m_ppTextures(rhs.m_ppTextures)
{
	for (_ulong i = 0; i < rhs.m_dwSubSetCnt; ++i)
	{
		m_ppTextures[i] = rhs.m_ppTextures[i];
		m_ppTextures[i]->AddRef();
	}

	m_pAdjacency->AddRef();
	m_pSubSetBuffer->AddRef();
	m_pOriMesh->AddRef();
	m_pMesh->AddRef();
}

Engine::CStaticMesh::~CStaticMesh(void)
{

}

HRESULT Engine::CStaticMesh::Initialize(const _tchar* FilePath, const _tchar* FileName)
{
	_tchar szFullPath[128] = L"";

	lstrcpy(szFullPath, FilePath);
	lstrcat(szFullPath, FileName);

	_ulong dwSubSetCnt = 0;

	if (FAILED(D3DXLoadMeshFromX(szFullPath, D3DXMESH_MANAGED, m_pDevice, &m_pAdjacency, &m_pSubSetBuffer, NULL, &dwSubSetCnt, &m_pOriMesh)))
		return E_FAIL;

	_ulong dwFVF = m_pOriMesh->GetFVF();

	if (dwFVF & D3DFVF_NORMAL)
	{
		m_pOriMesh->CloneMeshFVF(m_pOriMesh->GetOptions(), dwFVF, m_pDevice, &m_pMesh);
	}
	else
	{
		m_pOriMesh->CloneMeshFVF(m_pOriMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, m_pDevice, &m_pMesh);

		D3DXComputeNormals(m_pMesh, (_ulong*)m_pAdjacency->GetBufferPointer());
	}

	m_pSubSets = (D3DXMATERIAL*)m_pSubSetBuffer->GetBufferPointer();

	m_dwSubSetCnt = dwSubSetCnt == 0 ? 1 : dwSubSetCnt;

	m_ppTextures = new LPDIRECT3DTEXTURE9[m_dwSubSetCnt];

	if (dwSubSetCnt != 0)
	{
		for (_ulong i = 0; i < m_dwSubSetCnt; ++i)
		{
			_tchar szFileName[128] = L"";

			lstrcpy(szFullPath, FilePath);

			MultiByteToWideChar(CP_ACP, 0, m_pSubSets[i].pTextureFilename, strlen(m_pSubSets[i].pTextureFilename), szFileName, 128);

			lstrcat(szFullPath, szFileName);

			if (FAILED(D3DXCreateTextureFromFile(m_pDevice, szFullPath, &m_ppTextures[i])))
				return E_FAIL;
		}
	}
	else
	{
		m_pSubSets[0].MatD3D.Diffuse = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		m_pSubSets[0].MatD3D.Specular = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);	
		m_pSubSets[0].MatD3D.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		m_pSubSets[0].MatD3D.Ambient = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		m_pSubSets[0].MatD3D.Power = 1.0f;

		m_ppTextures[0] = NULL;
	}

	return S_OK;
}

void Engine::CStaticMesh::Render(void)
{
	for (_ulong i = 0; i < m_dwSubSetCnt; ++i)
	{
		if (m_ppTextures[i] != NULL)
			m_pDevice->SetTexture(0, m_ppTextures[i]);
		m_pMesh->DrawSubset(i);
	}
}

void Engine::CStaticMesh::ComputeMesh( MyVec3* pMin, MyVec3* pMax )
{
	D3DVERTEXELEMENT9	Element[MAX_FVF_DECL_SIZE];
	ZeroMemory(Element, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);

	m_pMesh->GetDeclaration(Element);

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

	m_pMesh->LockVertexBuffer(0, (void**)&pVertex);

	D3DXComputeBoundingBox((MyVec3*)(pVertex + byOffset), m_pMesh->GetNumVertices(), D3DXGetFVFVertexSize(m_pMesh->GetFVF()), pMin, pMax);

	m_pMesh->UnlockVertexBuffer();

}

void Engine::CStaticMesh::Free(void)
{
	Engine::CMesh::Free();

	for (_ulong i = 0; i < m_dwSubSetCnt; ++i)
	{
		Engine::Safe_Release(m_ppTextures[i]);
	}

	_ulong dwRefCnt = 0;
	dwRefCnt = Engine::Safe_Release(m_pOriMesh);
	dwRefCnt = Engine::Safe_Release(m_pMesh);

	Engine::Safe_Release(m_pAdjacency);
	Engine::Safe_Release(m_pSubSetBuffer);

	if (m_bClone == false)
		Engine::Safe_DeleteArray(m_ppTextures);
	
}

CComponent* Engine::CStaticMesh::Clone(void)
{
	return new CStaticMesh(*this);
}

CStaticMesh* Engine::CStaticMesh::Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* FilePath, const _tchar* FileName)
{
	CStaticMesh* pStatic = new CStaticMesh(pDevice);

	if (FAILED(pStatic->Initialize(FilePath, FileName)))
	{
		MSG_BOX("StaticMesh Created Failed");
		Engine::Safe_Release(pStatic);
	}

	return pStatic;
}
