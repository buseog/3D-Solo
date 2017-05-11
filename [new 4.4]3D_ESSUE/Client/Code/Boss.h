#ifndef Boss_h__
#define Boss_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CBoss :public Engine::CGameObject
{
public:
	enum ANIMATIONID { Idle, StepFront, Dash, ShockWave, TripleAttack, JumpSamsh, Bombing, Damage, Dying, Death, Intro };

private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CMaterial*		m_pMtrCom;
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CCollisionBox*	m_pCollisionCom;
	Engine::CShader*		m_pShaderCom;

private:
	_float					m_fHp;
	_float					m_fFullHp;
	_float					m_fAttack;
	_float					m_fSpeed;
	_float					m_fDeathTime;
	MyVec4					m_vColor;
	

private:
	_bool					m_bSkill[10];
	MyMat*					m_pMatPlayer;
	_float					m_fPatternTime;
	_float					m_fRefreshTime;
	_uint					m_iStateID;
	_bool					m_bStartPattern;
	_bool					m_bBlur;
	_bool					m_bHide;
	_bool					m_bExceptCollision;
	_bool					m_bDie;
	_bool					m_bAuto;

private:
	void	Pattern(const _float& fDelta);
	void	StepPattern(const _float& fDelta);
	void	DashPattern(const _float& fDelta);
	void	ShockWavePattern(const _float& fDelta);
	void	TripleAttackPattern(const _float& fDelta);
	void	JumpSmashPattern(const _float& fDelta);
	void	BombingPattern(const _float& fDelta);
	void	IntroPattern(const _float& fDelta);

public:
	void	SetDamage(const _float& fAttack);
	_bool	GetHide(void) { return m_bHide; }
	_bool	GetExcept(void) { return m_bExceptCollision; }
	_float	GetHp(void) { return m_fHp / m_fFullHp; }

private:
	HRESULT ReadyComponent(void);
	void	AnimationCheck(void);
	void	SetUpShader(LPD3DXEFFECT pEffect);
	
public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual _int	LastUpdate(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CBoss* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CBoss(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CBoss(void);
};

#endif // Boss_h__