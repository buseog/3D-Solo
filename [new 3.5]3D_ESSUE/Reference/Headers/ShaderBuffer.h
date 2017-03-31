#ifndef ShaderBuffer_h__
#define ShaderBuffer_h__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CShaderBuffer :public CVIBuffer
{
public:
	virtual HRESULT Initialize(const _uint& iSizeX, const _uint& iSizeY);
	virtual void Render(void);

private:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CShaderBuffer* Create(LPDIRECT3DDEVICE9 pDevice, const _uint& iSizeX, const _uint& iSizeY);

private:
	explicit CShaderBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShaderBuffer(const CShaderBuffer& rhs);
	virtual ~CShaderBuffer(void);
};

END
#endif // ShaderBuffer_h__