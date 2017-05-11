#ifndef BossShield_h__
#define BossShield_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CBoss;
class CBossShield :public Engine::CGameObject
{
private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CMaterial*		m_pMtrCom;
	Engine::CStaticMesh*	m_pMeshCom;
	Engine::CShader*		m_pShaderCom;

	MyMat*					m_pMatParent;
	MyMat*					m_pMatAttachment;
	MyMat					m_MatCompute;

private:
	HRESULT	ReadyComponent(void);
	void	SetUpShader(LPD3DXEFFECT pEffect);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CBossShield* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CBossShield(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CBossShield(void);
};

#endif // BossShield_h__