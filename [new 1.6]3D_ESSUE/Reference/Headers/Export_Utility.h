#ifndef Export_Utility_h__
#define Export_Utility_h__

#include "Engine_BigHeaders.h"
#include "Management.h"
#include "ComponentMgr.h"

#include "Scene.h"
#include "Layer.h"

#include "GameObject.h"

#include "Renderer.h"
#include "Transform.h"
#include "Texture.h"
#include "RcTex.h"
#include "TerrainTex.h"
#include "CubeCol.h"
#include "CubeTex.h"
#include "Material.h"
#include "Mesh.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"

BEGIN(Engine)

inline CManagement* Get_Management(void);
inline CComponentMgr*	Get_ComponentMgr(void);


inline void Release_Utility(void);

#include "Export_Utility.inl"

END
#endif // Export_Utility_h__
