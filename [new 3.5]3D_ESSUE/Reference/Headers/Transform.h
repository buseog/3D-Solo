#ifndef Transform_h__
#define Transform_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform :public CComponent
{
public:
	enum INFORMATION { INFO_SCALE, INFO_ANGLE, INFO_POS, INFO_END };
	enum ANGLEID { ANGLE_X, ANGLE_Y, ANGLE_Z };

private:
	LPDIRECT3DDEVICE9	m_pDevice;
	MyVec3				m_vInfo[INFO_END];
	MyMat				m_matWorld;
	MyMat*				m_pParent;
	_ulong				m_dwIndex;

public:
	void SetUpDevice(void);
	void SetUpShader(LPD3DXEFFECT pEffect);
	void SetInfomation(INFORMATION eInfo, const D3DXVECTOR3* pVec, const _uint& iOption = 0);
	void SetRotation(ANGLEID eAngle, const _float& fAngle);
	void SetParentMatrix(MyMat* pMat);
	void SetNaviIndex(const _ulong& dwIndex) { m_dwIndex = dwIndex; }
	MyVec3*	GetInfomation(INFORMATION eInfo) { return &m_vInfo[eInfo]; }
	D3DXMATRIX*	GetWorldMatrix(void);

public:
	void Move(const MyVec3* pDir, const _float& fSpeedDelta);
	void Straight(const MyVec3* pDir, const _float& fSpeedDelta);

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