#ifndef DarkKnight_h__
#define DarkKnight_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CDarkKnight :public CEffect
{
private:
	MyVec3	m_vAngle;
	_float	m_fRadius;
	_float	m_fRadiusDelta;
	_int	m_iCountDelta;

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
	static CDarkKnight* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType);

private:
	explicit CDarkKnight(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CDarkKnight(void);
};

#endif // DarkKnight_h__