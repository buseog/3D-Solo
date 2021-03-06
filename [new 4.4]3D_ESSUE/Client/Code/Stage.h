#ifndef Stage_h__
#define Stage_h__

#include "Export_Engine.h"
#include "BigHeaders.h"
#include "Scene.h"

class CLoading;
class CStage :public Engine::CScene
{
private:
	Engine::CCameraMgr*		m_pCamMgr;
	_bool					m_bStop;
	_bool					m_bNaviDraw;

private:
	HRESULT Ready_RenderState(void);
	HRESULT Ready_Light(void);
	HRESULT Ready_LayerMap(void);
	HRESULT	Ready_LayerObstacle(void);
	HRESULT	Ready_LayerSkyBox(void);
	HRESULT	Ready_LayerMonster(void);
	HRESULT Ready_LayerPlayer(void);
	HRESULT Ready_LayerETC(void);
	HRESULT	Ready_Camera(void);
	HRESULT	Ready_Navi(void);

public:
	virtual HRESULT Initialize(void);
	virtual _int Update(const _float& fDelta);
	virtual void Render(void);

public:
	virtual void Free(void);

public:
	static CStage* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CStage(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CStage(void);
};

#endif // Stage_h__