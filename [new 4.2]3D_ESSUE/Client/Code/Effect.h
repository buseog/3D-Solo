#ifndef Effect_h__
#define Effect_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CEffect :public Engine::CGameObject
{
protected:
	typedef list<EFFECT*>		EFFECTLIST;
	EFFECTLIST					m_EffectList;

protected:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CRcTex*				m_pBufferCom;
	Engine::CShader*			m_pShaderCom;

protected:
	MyMat*			m_pBone;
	MyMat*			m_pTarget;
	MyVec3			m_vOrigin;

	_tchar*			m_pEffectKey;
	_uint			m_iType;
	_uint			m_iIndex;

	_float			m_fDuration;
	_float			m_fInterval;
	_uint			m_iCount;

	_float			m_fDurationAcc;
	_float			m_fIntervalAcc;


protected:
	_float	GetSizeDelta(EFFECT* pEffect);
	_float	GetRollDelta(EFFECT* pEffect);
	_uint	GetAlphaDelta(EFFECT* pEffect);
	D3DXVECTOR3 GetScaleDelta(EFFECT* pEffect);
	D3DXVECTOR3 GetColorDelta(EFFECT* pEffect);
	bool	isDead(void);

public:
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

protected:
	explicit CEffect(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CEffect(void);
};

#endif // Effect_h__