#ifndef SwingDust_h__
#define SwingDust_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CSwingDust :public CEffect
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
	static CSwingDust* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType);

private:
	explicit CSwingDust(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CSwingDust(void);
};

#endif // SwingDust_h__