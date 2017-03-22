#ifndef Scene_h__
#define Scene_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class CLayer;
class ENGINE_DLL CScene :public CBase
{
private:
	typedef map<const _tchar*, CLayer*>	MAPLAYER;
	MAPLAYER						m_mapLayer;

protected:
	LPDIRECT3DDEVICE9 m_pDevice;

protected:
	CLayer* FindLayer(const _tchar* pLayerKey);

public:
	HRESULT	AddLayer(const _tchar* pLayerKey, CLayer* pLayer);
	CLayer*	GetLayer(const _tchar* LayerKey);

public:
	LPDIRECT3DDEVICE9 GetDevice(void);

public:
	virtual HRESULT Initialize(void);
	virtual _int Update(const _float& fDelta);
	virtual void Render(void) PURE;

protected:
	virtual void Free(void);

protected:
	explicit CScene(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CScene(void);
};

END
#endif // Scene_h__