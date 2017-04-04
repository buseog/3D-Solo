#ifndef Engine_struct_h__
#define Engine_struct_h__

BEGIN(Engine)

struct VTX_INSTANCE
{
	D3DCOLOR color;
	BYTE x;
	BYTE y;
	BYTE z;
	BYTE rotation;
};

typedef struct TagParticle
{
	D3DXVECTOR3	vPos;
	D3DCOLOR	vColor;

}PARTICLE;

const DWORD PARTICLEFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

typedef	struct	TagAttribute
{
	D3DXVECTOR3	vPos;			// ��ƼŬ ��ġ
	D3DXVECTOR3	vVelocity;		// ��ƼŬ �ӵ� ( /s)
	D3DXVECTOR3	vAcceleration;	// ��ƼŬ ���ӵ� ( /s)
	float		fSpeed;			// ��ƼŬ �̵��ӵ�
	float		fLifeTime;		// ��ƼŬ �����ð�
	float		fAge;			// ��ƼŬ ����
	D3DXCOLOR	vColor;			// ��ƼŬ �÷�
	D3DXCOLOR	vColorFade;		// ��ƼŬ �÷��� �ð��� ���� ����ϴ� ���
	bool		bAlive;			// ��ƼŬ ��������
}ATTRIBUTE;

typedef struct tagVertex_Screen_Texture
{
	D3DXVECTOR4			vPosition;
	D3DXVECTOR2			vTexUV;
}VTXSCREENTEX;

const unsigned long D3DFVF_VTXSCREENTEX = D3DFVF_XYZRHW | D3DFVF_TEX1;

typedef struct tagVertex_Texture
{
	D3DXVECTOR3		vPos;
	D3DXVECTOR2		vTexUV;
}VTXTEX;

const unsigned long D3DFVF_VTXTEX = D3DFVF_XYZ | D3DFVF_TEX1;

typedef struct tagVertex_Cube_Color
{
	D3DXVECTOR3		vPos;
	DWORD			vColor;
}VTXCOL;

const unsigned long D3DFVF_VTXCOL = D3DFVF_XYZ | D3DFVF_DIFFUSE;

typedef struct tagVertex_Normal_Texture
{
	D3DXVECTOR3		vPos;
	D3DXVECTOR3		vNormal;
	D3DXVECTOR2		vTexUV;
}VTXNORMALTEX;

const unsigned long D3DFVF_VTXNORMALTEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

typedef struct tagVertex_Cube_Texture
{
	D3DXVECTOR3		vPos;
	D3DXVECTOR3		vTexUV;
}VTXCUBETEX;

const unsigned long D3DFVF_VTXCUBETEX = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

typedef struct tagIndex16
{
	unsigned short _0;
	unsigned short _1;
	unsigned short _2;
}INDEX16;

typedef struct tagIndex32
{
	unsigned long _0;
	unsigned long _1;
	unsigned long _2;
}INDEX32;

typedef struct tagIndexTrail
{
	unsigned long _0;
	unsigned long _1;
	unsigned long _2;
	unsigned long _3;
}INDEXTrail;

typedef struct tagD3DXFrameDerived : public D3DXFRAME
{
	D3DXMATRIX		CombineTransformationMatrix;

}D3DXFRAME_DERIVED;

typedef struct tagD3DXMeshContainerDerived : public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9*		pTextures;
	LPD3DXMESH				pOriMesh;

	DWORD					dwBoneCnt;

	D3DXMATRIX**			ppCombineTransformationMatrices;
	D3DXMATRIX*				pOffsetMatrices;
	D3DXMATRIX*				pRenderingMatrices;

}D3DXMESHCONTAINER_DERIVED;

END
#endif // Engine_struct_h__
