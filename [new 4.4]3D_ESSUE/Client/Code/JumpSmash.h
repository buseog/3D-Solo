#ifndef JumpSmash_h__
#define JumpSmash_h__

#include "Effect.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CJumpSmash :public CEffect
{
private:
	MyMat*		m_pPlayer;
	MyVec3		m_vLook;
	_bool		m_bColl;

private:
	void DefaultSetting(void);
	void AddParticle(const _int& iIndex);

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
	static CJumpSmash* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iType);

private:
	explicit CJumpSmash(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CJumpSmash(void);
};

#endif // JumpSmash_h__