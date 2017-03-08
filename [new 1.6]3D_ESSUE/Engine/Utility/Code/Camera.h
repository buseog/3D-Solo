#ifndef Camera_h__
#define Camera_h__

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera :public CGameObject
{
protected:
	MyVec3		m_vEye, m_vAt, m_vUp;
	MyMat		m_matView;

	_float		m_fFovy, m_fAspect, m_fNear, m_fFar;
	MyMat		m_matProj;

public:
	virtual HRESULT	Initialize(void);
	virtual _int	Update(const _float& fDelta);

public:
	virtual void Free(void);

protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CCamera(void);
};

END
#endif // Camera_h__