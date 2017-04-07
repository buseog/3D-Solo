#ifndef ManaBlade_h__
#define ManaBlade_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CManaBlade :public CEffect
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
	virtual HRESULT Initialize(const _uint& iType, MyMat* pTarget, MyMat* pBone);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CManaBlade* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType, MyMat* pTarget, MyMat* pBone);

private:
	explicit CManaBlade(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CManaBlade(void);
};

#endif // ManaBlade_h__