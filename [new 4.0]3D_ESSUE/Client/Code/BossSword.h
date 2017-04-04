#ifndef BossSword_h__
#define BossSword_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CBoss;
class CBossSword :public Engine::CGameObject
{
private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CMaterial*		m_pMtrCom;
	Engine::CStaticMesh*	m_pMeshCom;
	Engine::CCollisionBox*	m_pCollisionCom;

	CBoss*					m_pBoss;
	MyMat*					m_pMatParent;
	MyMat*					m_pMatAttachment;
	MyMat					m_MatCompute;

private:
	HRESULT	ReadyComponent(void);

public:
	void	SetBoss(CBoss* pBoss);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CBossSword* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CBossSword(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CBossSword(void);
};

#endif // BossSword_h__