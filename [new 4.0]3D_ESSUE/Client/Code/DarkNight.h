#ifndef DarkNight_h__
#define DarkNight_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CDarkNight :public Engine::CGameObject
{
private:
	typedef list<EFFECT*>			EFFECTLIST;
	EFFECTLIST					m_DarkNightList;

private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CTrailTex*			m_pBufferCom;
	Engine::CShader*			m_pShaderCom;

private:
	_tchar*			m_pDarkNightKey;
	_uint			m_iType;
	_uint			m_iIndex;

	MyVec3*			m_pTarget;
	MyVec3			m_vOrigin;
	_float			m_fDuration;
	_float			m_fInterval;
	_uint			m_iCount;

	_float			m_fDurationAcc;
	_float			m_fIntervalAcc;

private:
	void DefaultSetting(void);
	void AddParticle(void);
	bool isDead(void);

private:
	virtual HRESULT Ready_Component(void);
	void	SetUpShader(LPD3DXEFFECT pEffect);

public:
	virtual HRESULT Initialize(const _uint& iType, MyVec3* pPos);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CDarkNight* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType, MyVec3* pPos);

private:
	explicit CDarkNight(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CDarkNight(void);
};

#endif // DarkNight_h__