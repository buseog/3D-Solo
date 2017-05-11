#ifndef DoomSayer_h__
#define DoomSayer_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CDoomSayer :public CEffect
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
	static CDoomSayer* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType, MyMat* pTarget, MyMat* pBone);

private:
	explicit CDoomSayer(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CDoomSayer(void);
};

#endif // DoomSayer_h__