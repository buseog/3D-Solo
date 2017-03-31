#ifndef LongBlade_h__
#define LongBlade_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CPlayer;
class CLongBlade :public Engine::CGameObject
{
private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CMaterial*		m_pMtrCom;
	Engine::CStaticMesh*	m_pMeshCom;
	Engine::CCollisionBox*	m_pCollisionCom;
	Engine::CCollisionBox*	m_pTargetCollision;
	Engine::CShader*		m_pShaderCom;

	MyMat*					m_pMatParent;
	MyMat*					m_pMatAttachment;
	MyMat					m_MatCompute;

private:
	Engine::CShader*		m_pBlurShaderCom;
	Engine::CShaderBuffer*	m_pShaderBufferCom;
	LPDIRECT3DTEXTURE9		m_pTargetTexture[2];
	LPDIRECT3DSURFACE9		m_pTargetSurface[2];
	_float					m_fRefreshTime;
	_float					m_fClearTime;


public:
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
	static CLongBlade* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CLongBlade(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CLongBlade(void);
};

#endif // LongBlade_h__