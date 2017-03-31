#include "Line2D.h"

USING(Engine)

CLine2D::CLine2D(void)
{
}

CLine2D::~CLine2D(void)
{
}

HRESULT Engine::CLine2D::Initialize( const MyVec3* pStart, const MyVec3* pEnd )
{
	m_vStart = MyVec2(pStart->x, pStart->z);
	m_vEnd = MyVec2(pEnd->x, pEnd->z);

	MyVec2 vDir = m_vEnd - m_vStart;
	m_vNormal = MyVec2(-vDir.y, vDir.x);
	D3DXVec2Normalize(&m_vNormal, &m_vNormal);

	return S_OK;
}

bool Engine::CLine2D::CheckLinePass( const MyVec2* pMoveEnd )
{
	MyVec2 vDir = *pMoveEnd - m_vStart;
	D3DXVec2Normalize(&vDir, &vDir);

	_float fDot = D3DXVec2Dot(&vDir, &m_vNormal);

	if (fDot > 0.f)
		return true;
	else
		return false;
}

void Engine::CLine2D::Free( void )
{

}

CLine2D* Engine::CLine2D::Create( const MyVec3* pStart, const MyVec3* pEnd )
{
	CLine2D* pLine = new CLine2D();

	if (FAILED(pLine->Initialize(pStart, pEnd)))
	{
		MSG_BOX("Line2D Created Failed");
		Engine::Safe_Release(pLine);
	}
	return pLine;
}
