#ifndef define_h__
#define define_h__

const unsigned int WINCX = 800;
const unsigned int WINCY = 600;

const D3DXVECTOR3 g_vLook	= D3DXVECTOR3(0.f, 0.f, 1.f);

enum SCENEID { SCENE_STATIC, SCENE_LOGO, SCENE_STAGE, SCENE_END};

enum STATEID { ST_NORMAL, ST_BATTLE, ST_ATTACK, ST_STUN, ST_END };

typedef struct PARTICLEVERTEX
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR2 vTexUV;
}PARTICLEVTX;

typedef struct PARTICLE_INSTANCE
{
	D3DCOLOR Color;
	D3DXVECTOR4 vRight;
	D3DXVECTOR4 vUp;
	D3DXVECTOR4 vLook;
	D3DXVECTOR4 vPos;
}PARTICLE_INSTANCE;

typedef struct ParticleEffect
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vScale;
	D3DXVECTOR3 vScaleDelta;
	D3DXVECTOR3 vAngle;
	D3DXVECTOR3 vAngleDelta;
	D3DXVECTOR3 vColor;
	D3DXVECTOR3 vColorDelta;

	float	fSpeed;
	float	fAngle;
	float	fRadius;
	float	fRadiusAngle;
	float	fLifeTime;
	float	fMaxLife;
	float	fFrame;
	float	fFrameSpeed;
	float	fGravity;

	float	fSizeDelta;
	float	fAlphaDelta;
	float	fRollDelta;

	float	fSizeWayTime;
	float	fSizeWayDelta;

	float	fAlphaWayTime;
	float	fAlphaWayDelta;

	float	fRollWayTime;
	float	fRollWayDelta;

	bool	bAlive;

}EFFECT;

typedef struct SaveObstacle
{
	TCHAR szKey[256];
	MyVec3 vScale;
	MyVec3 vAngle;
	MyVec3 vPos;
}SaveObstacle;

typedef struct SaveNavi
{
	MyVec3 vPointA;
	MyVec3 vPointB;
	MyVec3 vPointC;
}SaveNavi;

#endif // define_h__
