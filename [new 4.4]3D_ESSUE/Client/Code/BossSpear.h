#ifndef BossSpear_h__
#define BossSpear_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CBoss;
class CBossSpear :public Engine::CGameObject
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
	static CBossSpear* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CBossSpear(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CBossSpear(void);
};

#endif // BossSpear_h__