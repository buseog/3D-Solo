#ifndef Shader_h__
#define Shader_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader :public CComponent
{
private:
	LPDIRECT3DDEVICE9	m_pDevice;
	LPD3DXEFFECT		m_pEffect;
	LPD3DXBUFFER		m_pErrBuffer;

public:
	LPD3DXEFFECT	GetEffect(void) { return m_pEffect; }

public:
	HRESULT	Initialize(const _tchar* pFilePath);

public:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CShader* Create(LPDIRECT3DDEVICE9 pDevice, const _tchar* pFilePath);

private:
	explicit CShader(LPDIRECT3DDEVICE9 pDevice);
	explicit CShader(const CShader& rhs);
	virtual ~CShader(void);
};

END
#endif // Shader_h__