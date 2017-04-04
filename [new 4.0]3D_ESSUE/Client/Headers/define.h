#ifndef define_h__
#define define_h__

const unsigned int WINCX = 800;
const unsigned int WINCY = 600;

const D3DXVECTOR3 g_vLook	= D3DXVECTOR3(0.f, 0.f, 1.f);

enum SCENEID { SCENE_STATIC, SCENE_LOGO, SCENE_STAGE, SCENE_END};

enum STATEID { ST_NORMAL, ST_BATTLE, ST_ATTACK, ST_STUN, ST_END };

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

typedef struct ParticleEffect
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vScale;
	D3DXVECTOR3 vAngle;
	D3DXVECTOR3 vColor;
	float	fSpeed;
	float	fAngle;
	float	fRadius;
	float	fRadiusAngle;
	float	fLifeTime;
	float	fMaxLife;
	float	fFrame;
	float	fFrameSpeed;

	float	fSizeDelta;
	float	fAlphaDelta;

	float	fSizeWayTime;
	float	fSizeWayDelta;

	float	fAlphaWayTime;
	float	fAlphaWayDelta;

	float	fRollDelta;

	bool	bAlive;

}EFFECT;


#endif // define_h__
