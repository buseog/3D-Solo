#ifndef Effect_h__
#define Effect_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CTrailEffect :public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CTrailTex*			m_pBufferCom;
	Engine::CShader*			m_pShaderCom;
private:
	_float						m_fLifeTime;
	_float						m_fSaveTime;
private:
	D3DXVECTOR4					m_vecPos[40];
	_uint						m_iCount;
	_uint						m_iType;

public:
	void SetSaveTime(const _float& fSaveTime);
	void SetLifeTime(const _float& fLifeTime);

private:
	virtual HRESULT Ready_Component(void);
	void	SetUpShader(LPD3DXEFFECT pEffect);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CTrailEffect* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CTrailEffect(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CTrailEffect(void);
};

#endif // Effect_h__