#ifndef Logo_h__
#define Logo_h__

#include "BigHeaders.h"
#include "Scene.h"

class CLoading;
class CLogo :public Engine::CScene
{
private:
	CLoading*	m_pLoading;

private:
	HRESULT Ready_RenderState(void);
	HRESULT Ready_Light(void);
	HRESULT	Ready_Component(void);
	HRESULT Ready_LayerBackGround(void);

public:
	virtual HRESULT Initialize(void);
	virtual _int Update(const _float& fDelta);
	virtual void Render(void);

public:
	virtual void Free(void);

public:
	static CLogo* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CLogo(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CLogo(void);
};

#endif // Logo_h__