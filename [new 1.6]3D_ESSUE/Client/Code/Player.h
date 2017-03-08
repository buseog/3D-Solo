#ifndef Player_h__
#define Player_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CPlayer :public Engine::CGameObject
{
private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CMaterial*		m_pMtrCom;
	Engine::CDynamicMesh*	m_pMeshCom;

private:
	HRESULT ReadyComponent(void);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CPlayer(void);
};

#endif // Player_h__