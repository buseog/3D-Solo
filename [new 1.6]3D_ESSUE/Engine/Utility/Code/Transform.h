#ifndef Transform_h__
#define Transform_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform :public CComponent
{
public:
	enum INFORMATION { INFO_SCALE, INFO_ANGLE, INFO_POS, INFO_END };

private:
	LPDIRECT3DDEVICE9	m_pDevice;
	D3DXVECTOR3			m_vInfo[INFO_END];
	D3DXMATRIX			m_matWorld;

public:
	void SetInfomation(INFORMATION eInfo, const D3DXVECTOR3* pVec);
	void SetWorld(void);

public:
	virtual HRESULT Initialize(void);
	virtual void	Update(void);

public:
	virtual void Free(void);
	virtual CTransform* Clone(void);

public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CTransform(LPDIRECT3DDEVICE9 pDevice);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform(void);
};

END
#endif // Transform_h__