#ifndef BackGround_h__
#define BackGround_h__

#include "Export_Engine.h"
#include "Include.h"

class CObstacle;
class CBackGround :public Engine::CGameObject
{
public:
	list<CObstacle*>			m_ObstacleList;

private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CStaticMesh*		m_pMeshCom;

public:
	void	AddObstacle(CObstacle* pObstacle);

private:
	HRESULT Ready_Component(void);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CBackGround* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CBackGround(void);
};

#endif // BackGround_h__