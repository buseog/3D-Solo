#ifndef BackGround_h__
#define BackGround_h__

#include "BigHeaders.h"
#include "GameObject.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CTexture;
	class CRcTex;
	class CShader;
}

class CBackGround :public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CRcTex*				m_pBufferCom;
	Engine::CShader*			m_pShaderCom;
	_float						m_fPercent;

public:
	void	SetPercent(const _float& fPercent) { m_fPercent = fPercent; }

private:
	HRESULT	Ready_Component(void);
	void	SetUpShader(LPD3DXEFFECT pEffect);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CBackGround* Create(LPDIRECT3DDEVICE9 pDevice);


private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CBackGround(void);
};

#endif // BackGround_h__