#ifndef DynamicCamera_h__
#define DynamicCamera_h__

#include "BigHeaders.h"
#include "Camera.h"

class CDynamicCamera :public Engine::CCamera
{
private:
	void	KeyCheck(const _float& fDelta);

public:
	virtual HRESULT Initialize(const MyVec3* pEye, const MyVec3* pAt, const MyVec3* pUp, _float fFovy, _float fAspect, _float fNear, _float fFar);
	virtual _int	Update(const _float& fDelta);

public:
	virtual void Free(void);

public:
	static CDynamicCamera* Create(LPDIRECT3DDEVICE9 pDevice, const MyVec3* pEye, const MyVec3* pAt, const MyVec3* pUp, 
		_float fFovy = D3DXToRadian(60.0f), _float fAspect = _float(WINCX) / WINCY, _float fNear = 0.1f, _float fFar = 1000.f);

private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CDynamicCamera(void);
};

#endif // DynamicCamera_h__