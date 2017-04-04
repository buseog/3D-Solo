#ifndef CameraMgr_h__
#define CameraMgr_h__

#include "Engine_BigHeaders.h"
#include "Base.h"
#include "Camera.h"

BEGIN(Engine)

class CCamera;
class ENGINE_DLL CCameraMgr :public CBase
{
	DECLARE_SINGLETON(CCameraMgr)

private:
	typedef map<const _tchar*, CCamera*>	MAPCAMERA;
	MAPCAMERA								m_MapCamera;

private:
	CCamera*				m_pPlayCam;

private:
	CCamera*	FindCamera(const _tchar* CameraKey);

public:
	void		AddCamera(const _tchar* CameraKey, CCamera* pCamera);
	void		SetCamera(const _tchar* CameraKey);
	CCamera*	GetCamera(const _tchar* CameraKey);
	const _tchar*	GetCurrentCameraKey(void);

public:
	MyVec3*		GetCameraEye(void) { return m_pPlayCam->GetEye(); }
	MyVec3*		GetCameraAt(void) { return m_pPlayCam->GetAt(); }

public:
	_int Update(const _float& fDelta);

public:
	virtual void Free(void);

private:
	explicit CCameraMgr(void);
	virtual ~CCameraMgr(void);
};

END
#endif // CameraMgr_h__