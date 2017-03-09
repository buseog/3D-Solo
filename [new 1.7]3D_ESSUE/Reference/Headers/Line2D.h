#ifndef Line2D_h__
#define Line2D_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLine2D :public CBase
{
private:
	MyVec2	m_vStart;
	MyVec2	m_vEnd;
	MyVec2	m_vNormal;

public:
	HRESULT	Initialize(const MyVec3* pStart, const MyVec3* pEnd);
	bool	CheckLinePass(const MyVec2* pMoveEnd);

public:
	virtual void Free(void);

public:
	static CLine2D* Create(const MyVec3* pStart, const MyVec3* pEnd);

private:
	explicit CLine2D(void);
	virtual ~CLine2D(void);
};

END
#endif // Line2D_h__