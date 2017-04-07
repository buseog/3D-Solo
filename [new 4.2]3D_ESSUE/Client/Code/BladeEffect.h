#ifndef BladeEffect_h__
#define BladeEffect_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CBladeEffect :public CEffect
{
private:
	MyVec3	m_vAngle;
	MyMat*	m_pMatCompute;

private:
	void DefaultSetting(void);
	void AddParticle(const _int& iIndex);

private:
	virtual HRESULT Ready_Component(void);
	void	SetUpShader(LPD3DXEFFECT pEffect);

public:
	virtual HRESULT Initialize(const _uint& iType);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CBladeEffect* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType);

private:
	explicit CBladeEffect(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CBladeEffect(void);
};

#endif // BladeEffect_h__