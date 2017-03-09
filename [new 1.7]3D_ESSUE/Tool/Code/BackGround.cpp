#include "StdAfx.h"
#include "BackGround.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pDevice)
: m_pDevice(pDevice)
{

}

CBackGround::~CBackGround(void)
{
}

HRESULT CBackGround::Initialize(void)
{
	return S_OK;
}

CBackGround* CBackGround::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CBackGround* pBack = new CBackGround(pDevice);

	//if (FAILED(pBack->Initialize()))
		//Engine::Safe_Delete(pBack);

	return pBack;
}
