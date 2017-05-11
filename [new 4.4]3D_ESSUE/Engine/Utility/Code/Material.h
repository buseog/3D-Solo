#ifndef Material_h__
#define Material_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CMaterial :public CComponent
{
public:
	enum MATERIALID { MTR_DIFFUSE, MTR_AMBIENT, MTR_SPECULAR, MTR_EMISSIVE, MTR_POWER };

private:
	LPDIRECT3DDEVICE9	m_pDevice;

private:
	D3DMATERIAL9	m_MtrInfo;

public:
	void	SetMaterial(void);
	void	ChangeMaterial(MATERIALID eID, void* pValue);
	void	SetUpShader(LPD3DXEFFECT pEffect);

public:
	virtual HRESULT Initialize(const D3DXCOLOR& Diffuse, const D3DXCOLOR& Ambient, const D3DXCOLOR& Specular, const D3DXCOLOR& Emissive, const _float& fPower);

public:
	virtual void Free(void);
	virtual CComponent* Clone(void);

public:
	static CMaterial* Create(LPDIRECT3DDEVICE9 pDevice, const D3DXCOLOR& Diffuse, const D3DXCOLOR& Ambient, const D3DXCOLOR& Specular, const D3DXCOLOR& Emissive, const _float& fPower);

private:
	explicit CMaterial(LPDIRECT3DDEVICE9 pDevice);
	explicit CMaterial(const CMaterial& rhs);
	virtual ~CMaterial(void);
};

END
#endif // Material_h__