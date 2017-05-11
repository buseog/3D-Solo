#ifndef ShockWave_h__
#define ShockWave_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CShockWave :public CEffect
{
private:
	MyVec3	m_vAngle;

private:
	void DefaultSetting(void);
	void AddParticle(void);

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
	static CShockWave* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType);

private:
	explicit CShockWave(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CShockWave(void);
};

#endif // ShockWave_h__