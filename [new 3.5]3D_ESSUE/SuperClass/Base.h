#ifndef Base_h__
#define Base_h__

class _declspec(dllexport) CBase
{
protected:
	unsigned long m_dwRefCnt;

protected:
	virtual	void Free(void) PURE;

public:
	inline unsigned long AddRef(void);
	inline unsigned long Release(void);

protected:
	inline explicit CBase(void) :m_dwRefCnt(0) {}
	inline virtual ~CBase(void) {}
};

#include "Base.inl"

#endif // Base_h__
