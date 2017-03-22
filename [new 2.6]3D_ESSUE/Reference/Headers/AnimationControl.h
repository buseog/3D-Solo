#ifndef AnimationControl_h__
#define AnimationControl_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAnimationControl :public CBase
{
private:
	LPDIRECT3DDEVICE9				m_pDevice;
	LPD3DXANIMATIONCONTROLLER		m_pAnimationControl;
private:
	_ulong							m_dwTrack;
	_ulong							m_dwNewTrack;
	_double							m_dTimeAcc;
	_uint							m_iIndex;
	_uint							m_iMaxAniSet;
	_double							m_dPeriod;
	_float							m_fSpeed;

public:
	HRESULT	Initialize(void);
	void	SetAnimation(const _uint& iIndex, const _float& fSpeed, const _double& dFrame);
	void	PlayAnimation(const _float& fDelta);
	_bool	CheckAnimation(void);
	_double ProgressAnimation(void);
	_uint	GetAniIndex(void) { return m_iIndex; }

	LPD3DXANIMATIONCONTROLLER* GetAnimationController(void) { return &m_pAnimationControl; }

public:
	virtual void Free(void);

public:
	static CAnimationControl* Create(LPDIRECT3DDEVICE9 pDevice);
	static CAnimationControl* Create(const CAnimationControl& rhs);

private:
	explicit CAnimationControl(LPDIRECT3DDEVICE9 pDevice);
	explicit CAnimationControl(const CAnimationControl& rhs);
	virtual ~CAnimationControl(void);
};

END
#endif // AnimationControl_h__