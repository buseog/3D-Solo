#ifndef ActionCamera_h__
#define ActionCamera_h__

#include "BigHeaders.h"
#include "Export_Engine.h"

class CDynamicMesh;
class CPlayer;
class CActionCamera :public Engine::CCamera
{
private:
	CPlayer*				m_pPlayer;
	Engine::CDynamicMesh*	m_pCamMesh;
	MyMat*					m_pBoneEye;
	MyMat*					m_pBoneAt;
	MyMat*					m_pBoneOrigin;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer, Engine::CDynamicMesh* pCamMesh, const MyVec3* pUp, _float fFovy, _float fAspect, _float fNear, _float fFar);
	virtual _int	Update(const _float& fDelta);

public:
	virtual void Free(void);

public:
	static CActionCamera* Create(LPDIRECT3DDEVICE9 pDevice, CPlayer* pPlayer, Engine::CDynamicMesh* pCamMesh, const MyVec3* pUp, 
		_float fFovy = D3DXToRadian(60.0f), _float fAspect = _float(WINCX) / WINCY, _float fNear = 0.1f, _float fFar = 1000.f);

private:
	explicit CActionCamera(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CActionCamera(void);
};

#endif // ActionCamera_h__