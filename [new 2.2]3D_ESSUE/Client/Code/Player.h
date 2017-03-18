#ifndef Player_h__
#define Player_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CPlayer :public Engine::CGameObject
{
public:
	enum ANIMATIONID { Battle_Idle, Run_Lower, Run_Sprint, Attack_01, Attack_02, Attack_03, LordOfMana };

private:
	Engine::CKeyMgr*		m_pKeyMgr;
private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CMaterial*		m_pMtrCom;
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CCollisionBox*	m_pCollisionCom;
	Engine::CShader*		m_pShaderCom;

private:
	_float					m_fSpeed;
	_uint					m_iStateID;
	_uint					m_iAttackCnt;

private:
	HRESULT ReadyComponent(void);
	void	KeyCheck(const _float& fDelta);
	void	AnimationCheck(void);
	void	SetUpShader(LPD3DXEFFECT pEffect);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CPlayer(void);
};

#endif // Player_h__