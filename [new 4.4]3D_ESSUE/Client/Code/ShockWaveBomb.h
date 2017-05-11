#ifndef ShockWaveBomb_h__
#define ShockWaveBomb_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CShockWaveBomb :public CEffect
{
private:
	MyMat*		m_pPlayer;
	_float		m_fRadiusDelta;
	_uint		m_iCountDelta;
	_bool		m_bColl;

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
	static CShockWaveBomb* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType);

private:
	explicit CShockWaveBomb(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CShockWaveBomb(void);
};

#endif // ShockWaveBomb_h__