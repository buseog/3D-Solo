#ifndef Obstacle_h__
#define Obstacle_h__

#include "GameObject.h"
#include "Include.h"
#include "Export_Engine.h"

class CObstacle :public Engine::CGameObject
{
public:
	_tchar szKey[256];

public:
	Engine::CTransform*		m_pTransformCom;
	Engine::CStaticMesh*	m_pMeshCom;
	Engine::CCollisionBox*	m_pCollisionCom;

public:
	virtual HRESULT Initialize(const _tchar* pNameKey, const MyVec3* pPos);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CObstacle*	Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pNameKey, const MyVec3* pPos);

private:
	explicit CObstacle(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CObstacle(void);
};

#endif // Obstacle_h__