#ifndef Damage_h__
#define Damage_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CDamage :public CEffect
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
	virtual HRESULT Initialize(const _uint& iType, const MyVec3& vPos);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CDamage* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType, const MyVec3& vPos);

private:
	explicit CDamage(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CDamage(void);
};

#endif // Damage_h__