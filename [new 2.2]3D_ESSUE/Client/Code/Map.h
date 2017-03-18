#ifndef Map_h__
#define Map_h__

#include "BigHeaders.h"
#include "GameObject.h"
#include "Export_Engine.h"

class CMap :public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CMaterial*			m_pMtrCom;
	Engine::CStaticMesh*		m_pMeshCom;
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
	static CMap* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CMap(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CMap(void);
};

#endif // Map_h__