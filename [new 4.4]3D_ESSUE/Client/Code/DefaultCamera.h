#ifndef DefaultCamera_h__
#define DefaultCamera_h__


#include "BigHeaders.h"
#include "Camera.h"

class CDefaultCamera :public Engine::CCamera
{
public:
	enum ANGLEID { ANGLE_X, ANGLE_Y, ANGLE_Z, ANGLE_END };

private:
	MyMat*		m_pTarget;
	MyMat*		m_pCamBone;
	_float		m_fDistance;
	MyVec3		m_fAngle;
	_bool		m_bSkill;

private:
	void	KeyCheck(const _float& fDelta);
	void	SetUpCamera(void);

public:
	void	ActiveBone(_bool bTF) { m_bSkill = bTF; }
	MyVec3* GetAngle(void) { return &m_fAngle; }

public:
	virtual HRESULT Initialize(MyMat* pTarget, const MyVec3* pUp, _float fFovy, _float fAspect, _float fNear, _float fFar);
	virtual _int	Update(const _float& fDelta);

public:
	virtual void Free(void);

public:
	static CDefaultCamera* Create(LPDIRECT3DDEVICE9 pDevice, MyMat* pTarget, const MyVec3* pUp,
								_float fFovy = D3DXToRadian(60.0f), _float fAspect = _float(WINCX) / WINCY, _float fNear = 0.1f, _float fFar = 1000.f);

private:
	explicit CDefaultCamera(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CDefaultCamera(void);
};

#endif // DefaultCamera_h__ra_h__