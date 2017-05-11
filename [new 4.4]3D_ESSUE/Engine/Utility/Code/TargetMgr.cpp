#include "TargetMgr.h"
#include "Target.h"

USING(Engine)

IMPLEMENT_SINGLETON(CTargetMgr)

Engine::CTargetMgr::CTargetMgr( void )
{

}

Engine::CTargetMgr::~CTargetMgr( void )
{

}

HRESULT Engine::CTargetMgr::Init_Target( LPDIRECT3DDEVICE9 pDevice, const _tchar* pTargetKey, const _uint& iSizeX, const _uint& iSizeY, D3DFORMAT Format, D3DXCOLOR Color )
{
	CTarget* pTarget = FindTarget(pTargetKey);
	if (pTarget != NULL)
		return E_FAIL;

	pTarget = CTarget::Create(pDevice, iSizeX, iSizeY,	Format, Color);
	if (pTarget == NULL)
		return E_FAIL;

	m_mapTarget.insert(MAPTARGET::value_type(pTargetKey, pTarget));
	return S_OK;
}

HRESULT Engine::CTargetMgr::Init_MRT( const _tchar* pMRTKey, const _tchar* pTargetKey )
{
	CTarget* pTarget = FindTarget(pTargetKey);
	if (pTarget == NULL)
		return E_FAIL;

	list<CTarget*>* pMRTList = FindMRT(pMRTKey);
	if (pMRTList == NULL)
	{
		list<CTarget*> MRTList;
		MRTList.push_back(pTarget);
		m_mapMRT.insert(MAPMRT::value_type(pMRTKey, MRTList));
	}
	else
		pMRTList->push_back(pTarget);
	

	return S_OK;
}

HRESULT Engine::CTargetMgr::Init_Debug( const _tchar* pTargetKey, const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY )
{
	CTarget* pTarget = FindTarget(pTargetKey);
	if(NULL == pTarget)
		return E_FAIL;

	if(FAILED(pTarget->Initialize_Debug(fX, fY, fSizeX, fSizeY)))
		return E_FAIL;

	return S_OK;
}

void Engine::CTargetMgr::Render_Debug( const _tchar* pTargetKey )
{
	CTarget* pTarget = FindTarget(pTargetKey);
	if(NULL == pTarget)
		return;

	pTarget->Render_Debug();
}

HRESULT Engine::CTargetMgr::BeginMRT( const _tchar* pMRTKey, const _uint& iType )
{
	list<CTarget*>* pMRTList = FindMRT(pMRTKey);
	if (pMRTList == NULL)
		return E_FAIL;

	list<CTarget*>::iterator iter = pMRTList->begin();
	list<CTarget*>::iterator iter_end = pMRTList->end();

	if (iType == 0)
	{
		for ( ; iter != iter_end; ++iter)
			(*iter)->ClearTarget();
	}

	_uint iIndex = 0;

	for (iter = pMRTList->begin(); iter != iter_end; ++iter, ++iIndex)
		(*iter)->SetUpDevice(iIndex);

	return S_OK;
}

HRESULT Engine::CTargetMgr::EndMRT( const _tchar* pMRTKey )
{
	list<CTarget*>* pMRTList = FindMRT(pMRTKey);
	if (pMRTList == NULL)
		return E_FAIL;

	list<CTarget*>::iterator iter = pMRTList->begin();
	list<CTarget*>::iterator iter_end = pMRTList->end();

	_uint iIndex = 0;

	for ( ; iter != iter_end; ++iter, ++iIndex)
		(*iter)->ResetDevice(iIndex);

	return S_OK;
}

CTarget* Engine::CTargetMgr::FindTarget( const _tchar* pTargetKey )
{
	MAPTARGET::iterator iter = find_if(m_mapTarget.begin(), m_mapTarget.end(), CTagFinder(pTargetKey));

	if (iter == m_mapTarget.end())
		return NULL;

	return iter->second;
}

list<CTarget*>* Engine::CTargetMgr::FindMRT( const _tchar* pMRTKey )
{
	MAPMRT::iterator iter = find_if(m_mapMRT.begin(), m_mapMRT.end(), CTagFinder(pMRTKey));

	if (iter == m_mapMRT.end())
		return NULL;

	return &iter->second;
}

void Engine::CTargetMgr::SetUpShader(LPD3DXEFFECT pEffect, const _tchar* pTargetKey, const char* pTexKey)
{
	CTarget* pTarget = FindTarget(pTargetKey);

	if(NULL == pTarget)
		return;

	return pTarget->SetUpShader(pEffect, pTexKey);
}

CTarget* Engine::CTargetMgr::GetTarget( const _tchar* pTargetKey )
{
	CTarget* pTarget = FindTarget(pTargetKey);
	if(NULL == pTarget)
		return NULL;

	return pTarget;
}

void Engine::CTargetMgr::Free( void )
{
	for_each(m_mapTarget.begin(), m_mapTarget.end(), CReleasePair());
	m_mapTarget.clear();
}
