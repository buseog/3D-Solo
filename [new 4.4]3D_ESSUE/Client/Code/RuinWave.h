#ifndef RuinWave_h__
#define RuinWave_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CRuinWave :public CEffect
{
private:
	MyMat*	m_pBoss;	
	MyVec3	m_vAngle;
	_float	m_fDis;
	_bool	m_bColl;

private:
	void DefaultSetting(void);
	void AddParticle(const _uint& iIndex);

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
	static CRuinWave* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType, MyMat* pTarget, MyMat* pBone);

private:
	explicit CRuinWave(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CRuinWave(void);
};

#endif // RuinWave_h__