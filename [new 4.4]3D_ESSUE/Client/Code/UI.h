#ifndef UI_h__
#define UI_h__

#include "Export_Engine.h"
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

class CUI : public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CRcTex*				m_pBufferCom;
	Engine::CShader*			m_pShaderCom;

private:
	_float						m_fX, m_fY;
	_float						m_fSizeX, m_fSizeY;
	_float						m_fPercent;
	_uint						m_iType;
	_bool						m_bHide;

public:
	virtual HRESULT Initialize(const _uint& iType);
	virtual	_int	Update(const _float& fDelta);
	virtual void	Render(void);

private:
	HRESULT Ready_Component(void);

private:
	virtual void Free(void);

public:
	static CUI* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType);

private:
	explicit CUI(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CUI(void);
};


#endif // BackGround_Logo_h__
