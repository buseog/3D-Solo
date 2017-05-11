#ifndef ParticleSystem_h__
#define ParticleSystem_h__

#include "GameObject.h"
#include "Engine_BigHeaders.h"
#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CParticleSystem :public CGameObject
{
protected:
	CTexture*					m_pTextureCom;
	IDirect3DVertexBuffer9*		m_pVB;
	D3DXVECTOR3					m_vOrigin;		// 파티클이 시작되는장소

	float						m_fRate;		// 파티클의 추가되는 비율. 초당 파티클수
	float						m_fSize;		// 파티클의 크기
	int							m_iMaxCount;	// 정해진 시간동안 가질수 있는 파티클의 수. 파괴되는 속도보다 만들어지는 속도가 빨라져서 넘치는 현상을 막아줌.
	list<Engine::ATTRIBUTE*>	m_ParticleList;	// 파티클들을 관리할 리스트. 

	DWORD						m_dwVBSize;	// 버텍스 버퍼가 보관할 수 있는 파티클의 수. 이값은 파티클 시스템 내의 파티클 수와는 별개다
	DWORD						m_dwVBOffset;	// 버텍스 버퍼에서 복사할 파티클 내 다음 단계로의 오프셋(파티클단위)
	DWORD						m_dwVBBatchSize;	// 하나의 단계에 정의된 파티클 수


public:
	bool isEmpty(void);
	bool isDead(void);
	DWORD FToD(float f);

protected:
	virtual HRESULT	Initialize(void);
	virtual void	AddParticle(void);
	virtual void	RemoveDeadParticle(void);
	virtual void	ResetParticle(Engine::ATTRIBUTE* pAttribute) PURE;	// 시스템마다 지우는게 달라서 순수가상함수로만듬
	virtual void	preRender(void);
	virtual void	postRender(void);

public:
	virtual	_int	Update(const _float& fDelta) PURE;
	virtual void	Render(void);
	virtual void	Reset(void);
	
public:
	virtual void	Free(void);

protected:
	explicit CParticleSystem(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CParticleSystem(void);
};

END
#endif // ParticleSystem_h__