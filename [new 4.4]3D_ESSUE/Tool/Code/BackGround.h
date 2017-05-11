#ifndef BackGround_h__
#define BackGround_h__

#include "Export_Engine.h"
#include "Include.h"

class CObstacle;
class CCubeObj;
class CBackGround :public Engine::CGameObject
{
public:
	vector<CObstacle*>			m_vecObstacle;
	vector<CCubeObj*>			m_vecCube;
	CObstacle*					m_pPick;
	CCubeObj*					m_pPickCube;

private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CStaticMesh*		m_pMeshCom;

public:
	void	Picking(void);
	void	NaviPicking(void);
	void	KeyCheck(void);
	void	DeleteObstacle(const _uint& iIndex);

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