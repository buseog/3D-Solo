#ifndef Teleport_h__
#define Teleport_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CTeleport :public CEffect
{
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
	static CTeleport* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType);

private:
	explicit CTeleport(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CTeleport(void);
};

#endif // Teleport_h__