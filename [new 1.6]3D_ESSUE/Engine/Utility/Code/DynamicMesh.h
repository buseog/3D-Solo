#ifndef DynamicMesh_h__
#define DynamicMesh_h__

#include "Mesh.h"

BEGIN(Engine)

class CHierarchyLoad;
class CAnimationControl;
class ENGINE_DLL CDynamicMesh :public CMesh
{
private:
	CHierarchyLoad*		m_pLoader;
	CAnimationControl*	m_pAniController;
	LPD3DXFRAME			m_pRootFrame;
	MyMat				m_matParent;

private:
	typedef list<D3DXMESHCONTAINER_DERIVED*> MESHCONTAINERLIST; 
	MESHCONTAINERLIST	m_MeshContainerList;

public:
	void	UpdateCombineMatrix(D3DXFRAME_DERIVED* pFrame, const MyMat* pMatrix);
	void	SetUpMatrixAdress(D3DXFRAME_DERIVED* pRoot);

	void	SetAnimation(const _uint& iIndex);
	void	PlayAnimation(const _float& fDelta);

public:
	virtual HRESULT Initialize(const _tchar* FilePath, const _tchar* FileName);
	virtual void	Render(void);

public:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CDynamicMesh* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* FilePath, const _tchar* FileName);

private:
	explicit CDynamicMesh(LPDIRECT3DDEVICE9 pDevice);
	explicit CDynamicMesh(const CDynamicMesh& rhs);
	virtual ~CDynamicMesh(void);
};

END
#endif // DynamicMesh_h__