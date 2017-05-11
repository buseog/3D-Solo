#ifndef IntroCamera_h__
#define IntroCamera_h__

#include "BigHeaders.h"
#include "Export_Engine.h"

class CDynamicMesh;
class CBoss;
class CIntroCamera :public Engine::CCamera
{
private:
	CBoss*					m_pBoss;
	MyMat*					m_pBoneEye;
	Engine::CDynamicMesh*	m_pMesh;

public:
	virtual HRESULT Initialize(CBoss* pBoss, const MyVec3* pUp, _float fFovy, _float fAspect, _float fNear, _float fFar);
	virtual _int	Update(const _float& fDelta);

public:
	virtual void Free(void);

public:
	static CIntroCamera* Create(LPDIRECT3DDEVICE9 pDevice, CBoss* pBoss, const MyVec3* pUp, 
		_float fFovy = D3DXToRadian(60.0f), _float fAspect = _float(WINCX) / WINCY, _float fNear = 0.1f, _float fFar = 5000.f);

private:
	explicit CIntroCamera(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CIntroCamera(void);
};

#endif // IntroCamera_h__