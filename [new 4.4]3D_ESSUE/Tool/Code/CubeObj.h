#ifndef CubeObj_h__
#define CubeObj_h__

#include "GameObject.h"
#include "Include.h"
#include "Export_Engine.h"

class CCubeObj :public Engine::CGameObject
{
public:
	Engine::CTransform*		m_pTransformCom;
	Engine::CCubeCol*		m_pBufferCom;
	D3DXCOLOR				m_vColor;

public:
	void SetColor(D3DXCOLOR vColor);

public:
	virtual HRESULT Initialize(const MyVec3* pPos);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CCubeObj* Create(LPDIRECT3DDEVICE9 pDevice, const MyVec3* pPos);

private:
	explicit CCubeObj(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CCubeObj(void);
};

#endif // CubeObj_h__