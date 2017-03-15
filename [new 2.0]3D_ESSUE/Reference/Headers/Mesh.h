#ifndef Mesh_h__
#define Mesh_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CMesh :public CComponent
{
public:
	enum MESHTYPE { MESH_STATIC, MESH_DYNAMIC, MESH_NAVIGATION };

protected:
	LPDIRECT3DDEVICE9		m_pDevice;

protected:
	_bool					m_bClone;

public:
	virtual void Free(void);
	virtual CComponent* Clone(void) PURE;

protected:
	explicit CMesh(LPDIRECT3DDEVICE9 pDevice);
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh(void);
};

END
#endif // Mesh_h__