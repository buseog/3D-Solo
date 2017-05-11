#ifndef Player_h__
#define Player_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CSatellite;
class CPlayer :public Engine::CGameObject
{
public:
	enum ANIMATIONID { BattleIdle, RunLower, RunSprint, DiffusionShift, ManaBlade, Attack01, Attack02, Attack03, Attack04, Attack05,
		SharpenMana01, SharpenMana02, SharpenMana03, SharpenMana04, SharpenMana05, SharpenMana06, SharpenCombo, 
		DoomSayer, DoomSayerCombo, RuinBlade, LordOfMana, DarkKnight, DamageFront, DamageBack};

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
	_float					m_fHp;
	_float					m_fFullHp;
	_float					m_fAttack;
	_float					m_fSpeed;
	MyVec4					m_vColor;

private:
	_bool					m_bSkill[10];
	_bool					m_bManaBlade;
	_float					m_fBladeTime;
	_uint					m_iStateID;
	MyVec3					m_vPrevPos;
	_bool					m_bRim;

private:
	HRESULT ReadyComponent(void);
	void	SetUpShader(LPD3DXEFFECT pEffect);

private:
	void	KeyCheck(const _float& fDelta);
	void	Move(const _float& fDelta);
	void	Command(const _float& fDelta);
	void	Active(const _float& fDelta);
	void	Collider(const _float& fDelta);
	void	AnimationCheck(void);

public:
	void	SetDamage(const _float& fAttack);
	_uint&	GetState(void) { return m_iStateID; }

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