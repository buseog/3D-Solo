#ifndef Boss_h__
#define Boss_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CBoss :public Engine::CGameObject
{
public:
	enum ANIMATIONID { Idle, StepFront, Run, Dash, ShockWave, SpearThrow_Begin, SpearThrow_During, TripleAttack, Bombing};

private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CMaterial*		m_pMtrCom;
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CCollisionBox*	m_pCollisionCom;
	Engine::CShader*		m_pShaderCom;
	

private:
	MyMat*					m_pMatPlayer;
	_float					m_fSpeed;
	_uint					m_iStateID;
	_float					m_fPatternTime;

private:
	void	Pattern(const _float& fDelta);
	void	StepPattern(const _float& fDelta);
	void	RunPattern(const _float& fDelta);
	void	DashPattern(const _float& fDelta);
	void	ShockWavePattern(const _float& fDelta);
	void	TripleAttackPattern(const _float& fDelta);

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