#ifndef CamObserver_h__
#define CamObserver_h__

#include "Export_Engine.h"
#include "BigHeaders.h"

class CCamSubject;
class CCamObserver :public Engine::CObserver
{
private:
	CCamSubject*		m_pCamSubject;

private:
	D3DXVECTOR3		m_vEye;
	D3DXVECTOR3		m_vAt;

public:
	D3DXVECTOR3* GetEye(void);
	D3DXVECTOR3* GetAt(void);

public:
	virtual void Update(_int iMessage, void* pData);

public:
	virtual void Free(void);

private:
	explicit CCamObserver(void);
	virtual ~CCamObserver(void);
};

#endif // CamObserver_h__