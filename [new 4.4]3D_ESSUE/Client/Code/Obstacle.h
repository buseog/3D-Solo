#ifndef Obstacles_h__
#define Obstacles_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CObstacle :public Engine::CGameObject
{
private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CMaterial*		m_pMtrCom;
	Engine::CStaticMesh*	m_pMeshCom;
	Engine::CShader*		m_pShaderCom;

private:
	const _tchar*	m_pKey;
	_float			m_fOffset;

private:
	virtual HRESULT Ready_Component(const _tchar* pName);
	void	SetUpShader(LPD3DXEFFECT pEffect);

public:
	virtual HRESULT Initialize(const _tchar* pName);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CObstacle*	Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pName);

private:
	explicit CObstacle(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CObstacle(void);
};

#endif // Obstacles_h__