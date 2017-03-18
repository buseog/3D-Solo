#ifndef CollisionBox_h__
#define CollisionBox_h__

#include "Component.h"
#include "DynamicMesh.h"
#include "StaticMesh.h"

BEGIN(Engine)

class ENGINE_DLL CCollisionBox :public CComponent
{
private:
	typedef struct tagOBB
	{
		MyVec3	vCenter;
		MyVec3	vAxis[3];
		MyVec3	vPoint[3];
	}OBB;

public:
	enum COLLSTATE { COLL_FALSE, COLL_TRUE, COLL_END };
	enum COLLTYPE { COLL_AABB, COLL_OBB };

private:
	LPDIRECT3DDEVICE9			m_pDevice;
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;
	LPDIRECT3DTEXTURE9			m_pTexture[COLL_END];

private:
	OBB			m_tOBB;
	MyVec3		m_vMin, m_vMax;
	MyMat		m_MatLocal;
	MyMat		m_MatParent;

private:
	void SetOBB(void);

public:
	_bool CollisionAABB(CCollisionBox* pTargetBox);
	_bool CollisionOBB(CCollisionBox* pTargetBox);

public:
	virtual HRESULT Initialize(void);
	virtual void	Render(COLLTYPE eType, _bool bColl, const MyMat* pMatWorld);
	virtual HRESULT	ComputeBox(CStaticMesh* pMesh);
	virtual HRESULT	ComputeBox(CDynamicMesh* pMesh, const _uint& iIndex);

public:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CCollisionBox* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CCollisionBox(LPDIRECT3DDEVICE9 pDevice);
	explicit CCollisionBox(const CCollisionBox& rhs);
	virtual ~CCollisionBox(void);
};

END
#endif // CollisionBox_h__