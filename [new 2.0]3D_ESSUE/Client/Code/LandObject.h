#ifndef LandObject_h__
#define LandObject_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CLandObject :public Engine::CGameObject
{
protected:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CMaterial*		m_pMtrCom;
	Engine::CStaticMesh*	m_pMeshCom;

public:
	virtual void Free(void);

protected:
	explicit CLandObject(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CLandObject(void);
};


#endif // LandObject_h__