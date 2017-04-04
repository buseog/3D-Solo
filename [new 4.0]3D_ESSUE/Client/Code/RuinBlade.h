#ifndef RuinBlade_h__
#define RuinBlade_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CRuinBlade :public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CTrailTex*			m_pBufferCom;
	Engine::CShader*			m_pShaderCom;
private:
	_float						m_fLifeTime;

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
	static CRuinBlade* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CRuinBlade(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CRuinBlade(void);
};

#endif // RuinBlade_h__