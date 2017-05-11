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
	Engine::CDynamicMesh*	m_pMeshCom;
	Engine::CCollisionBox*	m_pCollisionCom;
	Engine::CCollisionBox*	m_pTargetCollision;
	Engine::CShader*		m_pShaderCom;

	MyMat*					m_pMatParent;
	MyMat*					m_pMatAttachment;
	MyMat					m_MatCompute;
	MyMat					m_MatBonus;

	_float					m_fCollisionTime;
	_bool					m_bAttachMent;
	_bool					m_bCollision;

public:
	void	SetAttach(_bool bTF);
	void	SetCollision(const _uint& iIndex = 0);
	MyMat*	GetComputeMatrix(void) { return &m_MatCompute; }

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
	static CBossSword* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CBossSword(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CBossSword(void);
};

#endif // BossSword_h__