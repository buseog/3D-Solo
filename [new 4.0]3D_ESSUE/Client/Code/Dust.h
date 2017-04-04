#ifndef Dust_h__
#define Dust_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CDust :public CEffect
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
	static CDust* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType, MyMat* pTarget, MyMat* pBone);

private:
	explicit CDust(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CDust(void);
};

#endif // Dust_h__