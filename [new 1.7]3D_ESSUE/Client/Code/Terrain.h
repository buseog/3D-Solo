#ifndef Terrain_h__
#define Terrain_h__

#include "BigHeaders.h"
#include "GameObject.h"
#include "Export_Engine.h"

class CTerrain :public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CMaterial*			m_pMtrCom;
	Engine::CStaticMesh*		m_pMeshCom;

private:
	virtual HRESULT Ready_Component(void);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CTerrain(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CTerrain(void);
};

#endif // Terrain_h__