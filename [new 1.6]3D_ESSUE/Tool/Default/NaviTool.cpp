// NaviTool.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "NaviTool.h"


// CNaviTool ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CNaviTool, CDialog)

CNaviTool::CNaviTool(CWnd* pParent /*=NULL*/)
	: CDialog(CNaviTool::IDD, pParent)
{

}

CNaviTool::~CNaviTool()
{
}

void CNaviTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNaviTool, CDialog)
END_MESSAGE_MAP()


// CNaviTool �޽��� ó�����Դϴ�.
