#ifndef define_h__
#define define_h__

const unsigned int WINCX = 800;
const unsigned int WINCY = 600;

const D3DXVECTOR3 g_vLook	= D3DXVECTOR3(0.f, 0.f, 1.f);

enum SCENEID { SCENE_STATIC, SCENE_LOGO, SCENE_STAGE, SCENE_END};

enum STATEID { ST_NORMAL, ST_BATTLE, ST_ATTACK, ST_STUN, ST_END };


#endif // define_h__
