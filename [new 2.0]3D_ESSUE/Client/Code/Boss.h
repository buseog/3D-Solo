#ifndef Boss_h__
#define Boss_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CBoss :public Engine::CGameObject
{
public:
	enum ANIMATIONID { Battle_Idle, Run_Lower, Run_Sprint, Attack_01, Attack_02, Attack_03, LordOfMana };

private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CMaterial*		m_pMtrCom;
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CCollisionBox*	m_pCollisionCom;

private:
	_float					m_fSpeed;
	_uint					m_iStateID;
	_uint					m_iAttackCnt;

private:
	HRESULT ReadyComponent(void);
	void	AnimationCheck(void);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
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