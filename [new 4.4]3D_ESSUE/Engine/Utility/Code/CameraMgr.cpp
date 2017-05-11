#include "CameraMgr.h"
#include "Camera.h"

USING(Engine)
IMPLEMENT_SINGLETON(CCameraMgr)

Engine::CCameraMgr::CCameraMgr(void)
: m_pPlayCam(NULL)
{
}

Engine::CCameraMgr::~CCameraMgr(void)
{
}

_int Engine::CCameraMgr::Update( const _float& fDelta )
{
	if (m_pPlayCam != NULL)
		m_pPlayCam->Update(fDelta);

	return 0;
}

CCamera* Engine::CCameraMgr::FindCamera( const _tchar* CameraKey )
{
	MAPCAMERA::iterator iter = find_if(m_MapCamera.begin(), m_MapCamera.end(), CTagFinder(CameraKey));

	if (iter == m_MapCamera.end())
		return NULL;

	return iter->second;
}

void Engine::CCameraMgr::AddCamera( const _tchar* CameraKey, CCamera* pCamera )
{
	CCamera* pFindCam = FindCamera(CameraKey);

	if (pFindCam != NULL)
		return;

	m_MapCamera.insert(MAPCAMERA::value_type(CameraKey, pCamera));
}

void Engine::CCameraMgr::SetCamera( const _tchar* CameraKey )
{
	CCamera* pFindCam = FindCamera(CameraKey);

	if (pFindCam == NULL)
		return;

	m_pPlayCam = pFindCam;
	m_pPlayCam->SetProjection();
}

CCamera* Engine::CCameraMgr::GetCamera( const _tchar* CameraKey )
{
	return FindCamera(CameraKey);
}

const _tchar* Engine::CCameraMgr::GetCurrentCameraKey(void)
{
	MAPCAMERA::iterator iter = m_MapCamera.begin();
	MAPCAMERA::iterator iter_end = m_MapCamera.end();

	for ( ; iter != iter_end; ++iter)
	{
		if (iter->second == m_pPlayCam)
		{
			return iter->first;
		}
	}
	return NULL;
}

void Engine::CCameraMgr::Free( void )
{
	for_each(m_MapCamera.begin(), m_MapCamera.end(), CReleasePair());
	m_MapCamera.clear();
}
