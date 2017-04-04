#ifndef CubeTex_h__
#define CubeTex_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCubeTex :public CVIBuffer
{
public:
	virtual HRESULT Initialize(void);
	virtual void	Render(void);

public:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CCubeTex* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CCubeTex(LPDIRECT3DDEVICE9 pDevice);
	explicit CCubeTex(const CCubeTex& rhs);
	virtual ~CCubeTex(void);
};

END

#endif // CubeTex_h__