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
	Engine::CShader*		m_pShader_Gaussian;
	Engine::CShader*		m_pShader_Refraction;
	Engine::CShader*		m_pShader_Light;
	Engine::CShader*		m_pShader_Blend;
	Engine::CShaderBuffer*	m_pShaderBuffer;
	_bool					m_bGaussian;
	_bool					m_bDebug;

private:
	void	Render_Priority(void);
	void	Render_NoneAlpha(void);
	void	Render_Alpha(void);
	void	Render_UI(void);
	void	Clear(void);

	void	Render_Deferred(void);
	void	Render_Light(void);
	void	Render_Blend(void);
	void	Render_Refraction(void);
	void	Render_Gaussian(void);

public:
	HRESULT	AddRenderList(RENDERTYPE eTpye, CGameObject* pGameObject);
	void	SetGaussian(_bool bTF) { m_bGaussian = bTF; }
	void	SetDebug(_bool bTF) { m_bDebug = bTF; }
	_bool	GetDebug(void) { return m_bDebug; }

	
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