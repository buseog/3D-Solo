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
	D3DXVECTOR3	vPos;			// 파티클 위치
	D3DXVECTOR3	vVelocity;		// 파티클 속도 ( /s)
	D3DXVECTOR3	vAcceleration;	// 파티클 가속도 ( /s)
	float		fSpeed;			// 파티클 이동속도
	float		fLifeTime;		// 파티클 유지시간
	float		fAge;			// 파티클 나이
	D3DXCOLOR	vColor;			// 파티클 컬러
	D3DXCOLOR	vColorFade;		// 파티클 컬러가 시간에 따라 퇴색하는 방법
	bool		bAlive;			// 파티클 생존여부
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
