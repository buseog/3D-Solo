#ifndef Dash_h__
#define Dash_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CDash :public CEffect
{
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
	static CDash* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType);

private:
	explicit CDash(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CDash(void);
};

#endif // Dash_h__