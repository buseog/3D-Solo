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
	D3DXVECTOR3					m_vOrigin;		// ��ƼŬ�� ���۵Ǵ����

	float						m_fRate;		// ��ƼŬ�� �߰��Ǵ� ����. �ʴ� ��ƼŬ��
	float						m_fSize;		// ��ƼŬ�� ũ��
	int							m_iMaxCount;	// ������ �ð����� ������ �ִ� ��ƼŬ�� ��. �ı��Ǵ� �ӵ����� ��������� �ӵ��� �������� ��ġ�� ������ ������.
	list<Engine::ATTRIBUTE*>	m_ParticleList;	// ��ƼŬ���� ������ ����Ʈ. 

	DWORD						m_dwVBSize;	// ���ؽ� ���۰� ������ �� �ִ� ��ƼŬ�� ��. �̰��� ��ƼŬ �ý��� ���� ��ƼŬ ���ʹ� ������
	DWORD						m_dwVBOffset;	// ���ؽ� ���ۿ��� ������ ��ƼŬ �� ���� �ܰ���� ������(��ƼŬ����)
	DWORD						m_dwVBBatchSize;	// �ϳ��� �ܰ迡 ���ǵ� ��ƼŬ ��


public:
	bool isEmpty(void);
	bool isDead(void);
	DWORD FToD(float f);

protected:
	virtual HRESULT	Initialize(void);
	virtual void	AddParticle(void);
	virtual void	RemoveDeadParticle(void);
	virtual void	ResetParticle(Engine::ATTRIBUTE* pAttribute) PURE;	// �ý��۸��� ����°� �޶� ���������Լ��θ���
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