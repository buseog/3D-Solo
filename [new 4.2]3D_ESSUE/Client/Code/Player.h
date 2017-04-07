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
		SharpenMana01, SharpenMana02, SharpenMana03, SharpenMana04, SharpenMana05, SharpenMana06, SharpenCombo, DoomSayer, DoomSayerCombo, RuinBlade, LordOfMana, DarkNight };

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
	Engine::CShader*		m_pBlurShaderCom;
	Engine::CShaderBuffer*	m_pShaderBufferCom;
	LPDIRECT3DTEXTURE9		m_pTargetTexture[2];
	LPDIRECT3DSURFACE9		m_pTargetSurface[2];

	_float					m_fTime;
	CSatellite*				m_pSatellite;

private:
	_bool					m_bSkill[5];
	_float					m_fSpeed;
	_uint					m_iStateID;
	MyVec3					m_vPrevPos;
	_uint					m_iEffectCnt;
	_float					m_fEffectCool;

private:
	HRESULT ReadyComponent(void);
	void	SetUpShader(LPD3DXEFFECT pEffect);

private:
	void	KeyCheck(const _float& fDelta);
	void	Move(const _float& fDelta);
	void	Command(const _float& fDelta);
	void	Active(const _float& fDelta);
	void	AddTrail(void);

	void	AnimationCheck(void);
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