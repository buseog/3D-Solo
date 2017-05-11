#ifndef Mountain_h__
#define Mountain_h__

#include "BigHeaders.h"
#include "GameObject.h"
#include "Export_Engine.h"

class CMountain :public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CMaterial*			m_pMtrCom;
	Engine::CStaticMesh*		m_pMeshCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CShader*			m_pShaderCom;

private:
	HRESULT Ready_Component(void);
	void	SetUpShader(LPD3DXEFFECT pEffect);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CMountain* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CMountain(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CMountain(void);
};

#endif // Mountain_h__