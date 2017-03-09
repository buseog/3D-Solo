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
	HRESULT	ReadyNaviMgr(LPDIRECT3DDEVICE9 pDevice);
	HRESULT	AddNaviMesh(const MyVec3* pVecA, const MyVec3* pVecB, const MyVec3* pVecC);
	void	RenderNaviMesh(void);
	void	LinkNaviMesh(void);

	_ulong	MoveOnNaviMesh(MyVec3* pPos, const MyVec3* pDir, const _ulong& dwIndex);

public:
	virtual void Free(void);

private:
	explicit CNaviMgr(void);
	virtual ~CNaviMgr(void);
};

END
#endif // NaviMgr_h__