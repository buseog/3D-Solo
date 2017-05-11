#ifndef Bombing_h__
#define Bombing_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CBombing :public CEffect
{
private:
	MyMat*		m_pPlayer;
	_bool		m_bColl;
	MyVec3		m_vAngle;
	_float		m_fRadius;
	_float		m_fRadiusDelta;
	_uint		m_iCountDelta;
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
	static CBombing* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType);

private:
	explicit CBombing(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CBombing(void);
};

#endif // Bombing_h__