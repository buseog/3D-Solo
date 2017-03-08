#ifndef SkyBox_h__
#define SkyBox_h__

#include "BigHeaders.h"
#include "GameObject.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CTexture;
	class CCubeTex;
}

class CSkyBox :public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CCubeTex*			m_pBufferCom;

private:
	virtual HRESULT	Ready_Component(void);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CSkyBox* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CSkyBox(void);
};

#endif // SkyBox_h__