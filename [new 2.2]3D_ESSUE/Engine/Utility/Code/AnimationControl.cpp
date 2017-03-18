#include "AnimationControl.h"

USING(Engine)

Engine::CAnimationControl::CAnimationControl( LPDIRECT3DDEVICE9 pDevice )
: m_pDevice(pDevice)
, m_pAnimationControl(NULL)
, m_dwTrack(0)
, m_dwNewTrack(1)
, m_dTimeAcc(0)
, m_iIndex(999)
, m_iMaxAniSet(0)
, m_dPeriod(0)
{
	m_pDevice->AddRef();
}

Engine::CAnimationControl::CAnimationControl( const CAnimationControl& rhs )
: m_pDevice(rhs.m_pDevice)
, m_dwTrack(rhs.m_dwTrack)
, m_dwNewTrack(rhs.m_dwNewTrack)
, m_dTimeAcc(rhs.m_dTimeAcc)
, m_iIndex(rhs.m_iIndex)
, m_iMaxAniSet(rhs.m_iMaxAniSet)
, m_dPeriod(rhs.m_dPeriod)
{

	rhs.m_pAnimationControl->CloneAnimationController(rhs.m_pAnimationControl->GetMaxNumAnimationOutputs(), m_iMaxAniSet = rhs.m_pAnimationControl->GetNumAnimationSets(), rhs.m_pAnimationControl->GetMaxNumTracks(), rhs.m_pAnimationControl->GetMaxNumEvents(), &m_pAnimationControl);
	m_pDevice->AddRef();
}

Engine::CAnimationControl::~CAnimationControl( void )
{

}

HRESULT Engine::CAnimationControl::Initialize( void )
{
	return S_OK;
}


void Engine::CAnimationControl::SetAnimation(const _uint& iIndex)
{
	if (m_iMaxAniSet <= iIndex)
		return;

	if (m_iIndex == iIndex)
		return;

	LPD3DXANIMATIONSET pAs = NULL;
	
	m_pAnimationControl->GetAnimationSet(iIndex, &pAs);

	if (pAs == NULL)
		return;

	m_dPeriod = pAs->GetPeriod();

	m_dwNewTrack = m_dwTrack == 0 ? 1 : 0;

	m_pAnimationControl->SetTrackAnimationSet(m_dwNewTrack, pAs);

	Engine::Safe_Release(pAs);

	m_pAnimationControl->UnkeyAllTrackEvents(m_dwTrack);
	m_pAnimationControl->UnkeyAllTrackEvents(m_dwNewTrack);

	m_pAnimationControl->KeyTrackEnable(m_dwTrack, FALSE, m_dTimeAcc + 0.2);
	m_pAnimationControl->KeyTrackWeight(m_dwTrack, 0.1f, m_dTimeAcc, 0.2, D3DXTRANSITION_LINEAR);
	m_pAnimationControl->KeyTrackSpeed(m_dwTrack, 1.f, m_dTimeAcc, 0.2, D3DXTRANSITION_LINEAR);

	m_pAnimationControl->SetTrackEnable(m_dwNewTrack, TRUE);
	m_pAnimationControl->KeyTrackWeight(m_dwNewTrack, 0.9f, m_dTimeAcc, 0.2, D3DXTRANSITION_LINEAR);
	m_pAnimationControl->KeyTrackSpeed(m_dwNewTrack, 1.f, m_dTimeAcc, 0.2, D3DXTRANSITION_LINEAR);

	m_pAnimationControl->ResetTime();
	m_dTimeAcc = 0;

	m_pAnimationControl->SetTrackPosition(m_dwNewTrack, 0);

	m_dwTrack = m_dwNewTrack;
	m_iIndex = iIndex;
}

void Engine::CAnimationControl::PlayAnimation( const _float& fDelta )
{
	m_pAnimationControl->AdvanceTime(fDelta, NULL);

	m_dTimeAcc += fDelta;
}


_bool Engine::CAnimationControl::EndAnimation( void )
{
	D3DXTRACK_DESC TrackDesc;
	ZeroMemory(&TrackDesc, sizeof(D3DXTRACK_DESC));
	m_pAnimationControl->GetTrackDesc(m_dwTrack, &TrackDesc);

	if ((TrackDesc.Position + 0.3) >= m_dPeriod)
		return true;

	return false;
}

void Engine::CAnimationControl::Free( void )
{
	Engine::Safe_Release(m_pDevice);
	Engine::Safe_Release(m_pAnimationControl);
}

CAnimationControl* Engine::CAnimationControl::Create( LPDIRECT3DDEVICE9 pDevice )
{
	CAnimationControl* pAniControl = new CAnimationControl(pDevice);

	if (FAILED(pAniControl->Initialize()))
	{
		MSG_BOX("AnimationControl Created Failed");
		Engine::Safe_Release(pAniControl);
	}
	return pAniControl;
}

CAnimationControl* Engine::CAnimationControl::Create( const CAnimationControl& rhs )
{
	CAnimationControl* pAniControl = new CAnimationControl(rhs);

	if (pAniControl == NULL)
	{
		MSG_BOX("AnimationControl Copy Failed");
		Engine::Safe_Release(pAniControl);
	}

	return pAniControl;
}
