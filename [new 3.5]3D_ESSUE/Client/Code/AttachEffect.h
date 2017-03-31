#ifndef AttachEffect_h__
#define AttachEffect_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CAttachEffect :public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CTrailTex*			m_pBufferCom;
	Engine::CShader*			m_pShaderCom;
private:
	MyMat*						m_pMatParent;
	MyMat*						m_pMatAttachment;
	MyMat						m_MatCompute;

	_tchar						m_szKey[256];
	_float						m_fLifeTime;
	_uint						m_iIndex;

public:
	void SetLifeTime(const _float& fLifeTime) { m_fLifeTime = fLifeTime; }
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
	static CAttachEffect* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pEffectKey, const _uint& iIndex = 0);

private:
	explicit CAttachEffect(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CAttachEffect(void);
};


#endif // AttachEffect_h__