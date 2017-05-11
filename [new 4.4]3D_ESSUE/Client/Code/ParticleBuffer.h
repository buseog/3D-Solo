#ifndef ParticleBuffer_h__
#define ParticleBuffer_h__

#include "GameObject.h"
#include "BigHeaders.h"
#include "Export_Engine.h"

class CParticleBuffer :public Engine::CGameObject
{
private:
	Engine::CTransform*			m_pTransformCom;
	Engine::CRenderer*			m_pRendererCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CShader*			m_pShaderCom;

	MyMat*						m_pTarget;
	_float						m_fTime;

private:
	LPDIRECT3DVERTEXBUFFER9			m_pVB;
	LPDIRECT3DINDEXBUFFER9			m_pIB;
	LPDIRECT3DVERTEXBUFFER9			m_pVBInstance;
	LPDIRECT3DVERTEXDECLARATION9    m_pVertexDecl;
	
private:
	HRESULT Ready_Component(void);

public:
	virtual HRESULT Initialize(void);
	virtual _int	Update(const _float& fDelta);
	virtual void	Render(void);

private:
	virtual void Free(void);

public:
	static CParticleBuffer* Create(LPDIRECT3DDEVICE9 pDevice);

private:
	explicit CParticleBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CParticleBuffer(void);
};

#endif // ParticleBuffer_h__