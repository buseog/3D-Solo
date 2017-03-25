// NaviTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "NaviTool.h"


// CNaviTool 대화 상자입니다.

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


// CNaviTool 메시지 처리기입니다.
