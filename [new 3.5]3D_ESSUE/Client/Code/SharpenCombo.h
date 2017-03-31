#ifndef SharpenCombo_h__
#define SharpenCombo_h__

#include "ParticleSystem.h"
#include "Export_Engine.h"
#include "BigHeaders.h"

class CSharpenCombo :public Engine::CParticleSystem
{
private:
	Engine::CRenderer*		m_pRendererCom;
	Engine::CShader*		m_pShaderCom;
	MyMat*					m_pMatTarget;
	_float					m_fLifeTime;
	MyVec3					m_vOrigin;
	MyVec3					m_vOriDir;
	_uint					m_iType;
	_uint					m_iMaxCount;
	_float					m_fRefresh;

public:
	void	SetUpShader(void);
	void	AddParticle(const _uint& iIndex);

private:
	HRESULT	ReadyComponent(void);
	void	ResetParticle(Engine::ATTRIBUTE* pAttribute) {};
	void	ResetParticle(Engine::ATTRIBUTE* pAttribute, const _uint& iIndex);

private:
	virtual void	preRender(void);

public:
	virtual HRESULT Initialize(MyMat* pMatTarget, const _uint& iType);
	virtual _int Update(const _float& fDelta);
	virtual void Render(void);

public:
	virtual void Free(void);

public:
	static CSharpenCombo*	Create(LPDIRECT3DDEVICE9 pDevice, MyMat* pMatTarget, const _uint& iType = 0);

private:
	explicit CSharpenCombo(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CSharpenCombo(void);
};

#endif // SharpenCombo_h__