#ifndef Water_h__
#define Water_h__

#include "BigHeaders.h"
#include "GameObject.h"
#include "Export_Engine.h"

class CWater :public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CMaterial*			m_pMtrCom;
	Engine::CStaticMesh*		m_pMeshCom;
	Engine::CShader*			m_pShaderCom;

private:
	_float						m_fOffset;
	_float						m_fTime;
	_float						m_fHeight;
	_float						m_fSpeed;
	_float						m_fFrequency;

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
	static CWater* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CWater(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CWater(void);
};

#endif // Water_h__