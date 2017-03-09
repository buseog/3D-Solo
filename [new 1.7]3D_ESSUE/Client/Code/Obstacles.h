#ifndef Obstacles_h__
#define Obstacles_h__

#include "LandObject.h"

class CObstacles :public CLandObject
{
private:
	virtual HRESULT Ready_Component(void);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

public:
	virtual void Free(void);

public:
	static CObstacles*	Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CObstacles(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CObstacles(void);
};

#endif // Obstacles_h__