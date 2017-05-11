#ifndef TargetMgr_h__
#define TargetMgr_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CTarget;
class ENGINE_DLL CTargetMgr :public CBase
{
	DECLARE_SINGLETON(CTargetMgr)

private:
	typedef map<const _tchar*, CTarget*>	MAPTARGET;
	MAPTARGET								m_mapTarget;

private:
	typedef map<const _tchar*, list<CTarget*>>	MAPMRT;
	MAPMRT										m_mapMRT;

private:
	CTarget*	FindTarget(const _tchar* pTargetKey);
	list<CTarget*>* FindMRT(const _tchar* pMRTKey);

public:
	HRESULT Init_Target(LPDIRECT3DDEVICE9 pDevice, const _tchar* pTargetKey, const _uint& iSizeX, const _uint& iSizeY, D3DFORMAT Format, D3DXCOLOR Color);
	HRESULT	Init_MRT(const _tchar* pMRTKey, const _tchar* pTargetKey);

	HRESULT BeginMRT(const _tchar* pMRTKey, const _uint& iType = 0);
	HRESULT EndMRT(const _tchar* pMRTKey);
	
	HRESULT Init_Debug(const _tchar* pTargetKey, const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);
	void Render_Debug(const _tchar* pTargetKey);

	void SetUpShader(LPD3DXEFFECT pEffect, const _tchar* pTargetKey, const char* pTexKey);
	CTarget* GetTarget(const _tchar* pTargetKey);

private:
	virtual void Free(void);

private:
	explicit CTargetMgr(void);
	virtual ~CTargetMgr(void);
};

END
#endif // TargetMgr_h__