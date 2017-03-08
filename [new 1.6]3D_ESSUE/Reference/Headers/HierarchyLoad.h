#ifndef HierarchyLoad_h__
#define HierarchyLoad_h__

#include "Engine_BigHeaders.h"

BEGIN(Engine)

class ENGINE_DLL CHierarchyLoad :public ID3DXAllocateHierarchy
{
private:
	LPDIRECT3DDEVICE9	m_pDevice;

private:
	const _tchar*		m_pFilePath;
	_ulong				m_dwRefCnt;

private:
	void AllocateName(char** ppSrcName, const char* pDestName);

public:
	virtual HRESULT Initialize(const _tchar* FilePath);
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name, CONST D3DXMESHDATA *pMeshData, CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer);
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree); 
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree);

public:
	_ulong		Release(void);
	_ulong		AddRef(void) { return ++m_dwRefCnt; }

public:
	static CHierarchyLoad* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* Filepath);

private:
	explicit CHierarchyLoad(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CHierarchyLoad(void);
};

END
#endif // HierarchyLoad_h__