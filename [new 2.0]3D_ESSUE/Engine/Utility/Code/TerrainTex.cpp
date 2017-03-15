#include "TerrainTex.h"

USING(Engine)

Engine::CTerrainTex::CTerrainTex( LPDIRECT3DDEVICE9 pDevice )
: CVIBuffer(pDevice)
, m_pPos(NULL)
, m_isClone(false)
{
}

Engine::CTerrainTex::CTerrainTex( const CTerrainTex& rhs )
: CVIBuffer(rhs)
, m_pPos(rhs.m_pPos)
, m_fh(rhs.m_fh)
, m_ih(rhs.m_ih)
, m_isClone(true)
{

}

Engine::CTerrainTex::~CTerrainTex( void )
{

}

HRESULT Engine::CTerrainTex::Initialize( const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwItv )
{
	m_dwVtxCnt = dwCntX * dwCntZ;
	m_pPos = new MyVec3[m_dwVtxCnt];

	m_dwVtxSize = sizeof(VTXNORMALTEX);
	m_dwVtxFVF = D3DFVF_VTXNORMALTEX;

	m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;
	m_dwIdxSize = sizeof(INDEX32) * m_dwTriCnt;
	m_IndexFmt = D3DFMT_INDEX32;

	if(FAILED(Engine::CVIBuffer::Initialize()))
		return E_FAIL;

	_ulong			dwByte = 0;

	m_hFile = CreateFile(L"../Bin/Texture/Terrain/Height.bmp", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	ReadFile(m_hFile, &m_fh, sizeof(BITMAPFILEHEADER), &dwByte, NULL);
	ReadFile(m_hFile, &m_ih, sizeof(BITMAPINFOHEADER), &dwByte, NULL);

	_ulong*			pPixel = new _ulong[m_ih.biWidth * m_ih.biHeight];

	ReadFile(m_hFile, pPixel, sizeof(_ulong) * m_ih.biWidth * m_ih.biHeight, &dwByte, NULL);

	CloseHandle(m_hFile);

	// 할당된 메모리에 접근을 하자.
	VTXNORMALTEX*				pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	_ulong		dwIndex = 0;

	for	(_ulong i = 0; i < dwCntZ; ++i)
	{
		for (_ulong j = 0; j < dwCntX; ++j)
		{
			dwIndex = i * dwCntX + j;

			pVertex[dwIndex].vPos = MyVec3(j * (_float)dwItv, (pPixel[dwIndex] & 0x000000ff) / 10.0f, i * (_float)dwItv);
			m_pPos[dwIndex] = pVertex[dwIndex].vPos;
			pVertex[dwIndex].vNormal = MyVec3(0.f, 0.f, 0.f);
			pVertex[dwIndex].vTexUV = MyVec2(_float(j) / (dwCntX - 1) * 20.f, _float(i) / (dwCntZ - 1) * 20.f);
		}
	}


	Engine::Safe_DeleteArray(pPixel);

	INDEX32*			pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	_ulong		dwTriIdx = 0;

	for	(_ulong i = 0; i < dwCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwCntX - 1; ++j)
		{
			dwIndex = i * dwCntX + j;

			pIndex[dwTriIdx]._0 = dwIndex + dwCntX;
			pIndex[dwTriIdx]._1 = dwIndex + dwCntX + 1;
			pIndex[dwTriIdx]._2 = dwIndex + 1;

			MyVec3		vDest, vSour, vNormal;
			vDest = pVertex[pIndex[dwTriIdx]._1].vPos - pVertex[pIndex[dwTriIdx]._0].vPos;
			vSour = pVertex[pIndex[dwTriIdx]._2].vPos - pVertex[pIndex[dwTriIdx]._1].vPos;
			D3DXVec3Cross(&vNormal, &vDest, &vSour);
			pVertex[pIndex[dwTriIdx]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriIdx]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriIdx]._2].vNormal += vNormal;
			++dwTriIdx;

			pIndex[dwTriIdx]._0 = dwIndex + dwCntX;
			pIndex[dwTriIdx]._1 = dwIndex + 1;
			pIndex[dwTriIdx]._2 = dwIndex;
			vDest = pVertex[pIndex[dwTriIdx]._1].vPos - pVertex[pIndex[dwTriIdx]._0].vPos;
			vSour = pVertex[pIndex[dwTriIdx]._2].vPos - pVertex[pIndex[dwTriIdx]._1].vPos;
			D3DXVec3Cross(&vNormal, &vDest, &vSour);
			pVertex[pIndex[dwTriIdx]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriIdx]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriIdx]._2].vNormal += vNormal;
			++dwTriIdx;
		}
	}

	for (_ulong i = 0; i < m_dwVtxCnt; ++i)	
		D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);


	m_pVB->Unlock();
	m_pIB->Unlock();

	return S_OK;
}

void Engine::CTerrainTex::Render( void )
{
	m_pDevice->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pDevice->SetFVF(m_dwVtxFVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
}

const MyVec3* Engine::CTerrainTex::GetVtxPos( void )
{
	return m_pPos;
}

void Engine::CTerrainTex::Free( void )
{
	Engine::CVIBuffer::Free();

	if(false == m_isClone)
	{
		Engine::Safe_DeleteArray(m_pPos);
	}
}

CComponent* Engine::CTerrainTex::Clone( void )
{
	return new CTerrainTex(*this);
}

CTerrainTex* Engine::CTerrainTex::Create( LPDIRECT3DDEVICE9 pDevice, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwItv )
{
	CTerrainTex* pTerrainTex = new CTerrainTex(pDevice);

	if (FAILED(pTerrainTex->Initialize(dwCntX, dwCntZ, dwItv)))
	{
		MSG_BOX("Terrain Created Failed");
		Engine::Safe_Release(pTerrainTex);
	}

	return pTerrainTex;
}
