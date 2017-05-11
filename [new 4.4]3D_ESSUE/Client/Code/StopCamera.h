#ifndef StopCamera_h__
#define StopCamera_h__

#include "BigHeaders.h"
#include "Export_Engine.h"

class CStopCamera :public Engine::CCamera
{
private:
	_float	m_fSpeed;
	_bool	m_bHold;

private:
	void	KeyCheck(const _float& fDelta);

public:
	void	SetCameraSetting(void);

public:
	virtual HRESULT Initialize(const MyVec3* pEye, const MyVec3* pAt, const MyVec3* pUp, _float fFovy, _float fAspect, _float fNear, _float fFar);
	virtual _int	Update(const _float& fDelta);

public:
	virtual void Free(void);

public:
	static CStopCamera* Create(LPDIRECT3DDEVICE9 pDevice, const MyVec3* pEye, const MyVec3* pAt, const MyVec3* pUp, 
		_float fFovy = D3DXToRadian(60.0f), _float fAspect = _float(WINCX) / WINCY, _float fNear = 0.1f, _float fFar = 5000.f);

private:
	explicit CStopCamera(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CStopCamera(void);
};

#endif // StopCamera_h__