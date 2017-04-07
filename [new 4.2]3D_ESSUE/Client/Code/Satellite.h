#ifndef Satellite_h__
#define Satellite_h__

#include "ParticleSystem.h"
#include "Export_Engine.h"
#include "BigHeaders.h"

class CSatellite :public Engine::CParticleSystem
{
private:
	Engine::CShader*		m_pShaderCom;
	MyMat*					m_pTargetPos;
	_float					m_fRefreshTime;

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
	static CSatellite*	Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pTexKey);

private:
	explicit CSatellite(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CSatellite(void);
};

#endif // Satellite_h__