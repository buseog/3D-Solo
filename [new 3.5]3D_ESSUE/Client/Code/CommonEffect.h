#ifndef CommonEffect_h__
#define CommonEffect_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CCommonEffect :public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CTrailTex*			m_pBufferCom;
	Engine::CShader*			m_pShaderCom;
private:
	_tchar						m_szKey[256];
	_float						m_fLifeTime;
	_uint						m_iIndex;
	_uint						m_iType;

public:
	void SetLifeTime(const _float& fLifeTime) { m_fLifeTime = fLifeTime; }
	void SetType(const _uint& itype) { m_iType = itype; }
	void SetInformation(const _uint& iIndex, const MyVec3* pVec);

private:
	virtual HRESULT Ready_Component(void);
	void	SetUpShader(LPD3DXEFFECT pEffect);

public:
	virtual HRESULT Initialize(const _tchar* pEffectKey, const _uint& iIndex);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CCommonEffect* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pEffectKey, const _uint& iIndex = 0);

private:
	explicit CCommonEffect(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CCommonEffect(void);
};


#endif // CommonEffect_h__