#ifndef Renderer_h__
#define Renderer_h__

#include "Component.h"

BEGIN(Engine)

class CGameObject;
class CShader;
class CShaderBuffer;
class ENGINE_DLL CRenderer :public CComponent
{
public:
	enum RENDERTYPE { RENDER_PRIORITY, RENDER_NONEALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };

private:
	LPDIRECT3DDEVICE9	m_pDevice;

private:
	typedef list<CGameObject*>	RENDERLIST;
	RENDERLIST		m_RenderList[RENDER_END];

private:
	Engine::CShader*		m_pShader;
	Engine::CShaderBuffer*	m_pShaderBuffer;
	LPDIRECT3DTEXTURE9		m_pTargetTexture[2];
	LPDIRECT3DSURFACE9		m_pTargetSurface[2];

private:
	void	Render_Priority(void);
	void	Render_NoneAlpha(void);
	void	Render_Alpha(void);
	void	Render_UI(void);
	void	Clear(void);

public:
	HRESULT	AddRenderList(RENDERTYPE eTpye, CGameObject* pGameObject);
	
public:
	virtual HRESULT Initialize(void);
	virtual void	Render(void);

public:
	virtual void Free(void);
	virtual CComponent* Clone(void);
	
public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CRenderer(void);
};

END
#endif // Renderer_h__