#ifndef NaviMgr_h__
#define NaviMgr_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CNaviMesh;
class ENGINE_DLL CNaviMgr :public CBase
{
	DECLARE_SINGLETON(CNaviMgr)

private:
	LPDIRECT3DDEVICE9		m_pDevice;
private:
	typedef vector<CNaviMesh*>	VECNAVI;
	VECNAVI						m_vecNavi;

	_ulong						m_dwIdxCnt;

public:
	void Picking(const _uint& iIndex, _bool bTF);

public:
	HRESULT	ReadyNaviMgr(LPDIRECT3DDEVICE9 pDevice);
	HRESULT	AddNaviMesh(const MyVec3* pVecA, const MyVec3* pVecB, const MyVec3* pVecC);
	void	RenderNaviMesh(void);
	void	LinkNaviMesh(void);

	void	MoveOnNaviMesh(MyVec3* pPos, const MyVec3* pDir, _ulong* pIndex);
	vector<CNaviMesh*>* GetNavi(void) { return &m_vecNavi; }

public:
	virtual void Free(void);

private:
	explicit CNaviMgr(void);
	virtual ~CNaviMgr(void);
};

END
#endif // NaviMgr_h__