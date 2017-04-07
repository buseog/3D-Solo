#ifndef ManaStone_h__
#define ManaStone_h__

#include "ParticleSystem.h"
#include "Export_Engine.h"
#include "BigHeaders.h"

class CManaStone :public Engine::CParticleSystem
{
private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CShader*		m_pShaderCom;

public:
	void	SetUpShader(void);

private:
	HRESULT	ReadyComponent(const _tchar* pTexkey);
	void	ResetParticle(Engine::ATTRIBUTE* pAttribute);

private:
	virtual void	preRender(void);

public:
	virtual HRESULT Initialize(const _tchar* pTexKey);
	virtual _int Update(const _float& fDelta);
	virtual void Render(void);

public:
	virtual void Free(void);

public:
	static CManaStone*	Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pTexKey);

private:
	explicit CManaStone(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CManaStone(void);
};

#endif // ManaStone_h__