#ifndef Boss_h__
#define Boss_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CBoss :public Engine::CGameObject
{
public:
	enum ANIMATIONID { Idle, StepFront, Run, Dash, JumpSmash, SpearThrow_During, SpearThrow_Begin, Bombing, ShockWave };

private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CMaterial*		m_pMtrCom;
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CCollisionBox*	m_pCollisionCom;
	MyMat*					m_pMatPlayer;

private:
	_float					m_fSpeed;
	_uint					m_iStateID;
	MyMat*					m_pMatBone;

private:
	HRESULT ReadyComponent(void);
	void	AnimationCheck(void);
	void	Pattern(const _float& fDelta);

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