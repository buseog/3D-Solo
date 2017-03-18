// MainForm.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "MainForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CMainForm

IMPLEMENT_DYNCREATE(CMainForm, CFormView)

CMainForm::CMainForm()
	: CFormView(CMainForm::IDD)
{

}

CMainForm::~CMainForm()
{
}

void CMainForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabControl);
}

BEGIN_MESSAGE_MAP(CMainForm, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMainForm::OnChangeTab)
END_MESSAGE_MAP()


// CMainForm �����Դϴ�.

#ifdef _DEBUG
void CMainForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMainForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMainForm �޽��� ó�����Դϴ�.

void CMainForm::OnChangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	UpdateData(TRUE);

	int iIndex = m_TabControl.GetCurSel();

	switch(iIndex)
	{
	case 0:
		m_MapTool.ShowWindow(SW_SHOW);
		m_NaviTool.ShowWindow(SW_HIDE);
		break;

	case 1:
		m_MapTool.ShowWindow(SW_HIDE);
		m_NaviTool.ShowWindow(SW_SHOW);
		break;
	}

	UpdateData(FALSE);
}

void CMainForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	m_TabControl.DeleteAllItems();
	m_TabControl.InsertItem(0, L"Map");
	m_TabControl.InsertItem(1, L"Navi Mesh");

	CRect rc;
	m_TabControl.GetClientRect(&rc);

	m_MapTool.Create(IDD_MAPTOOL, &m_TabControl );
	m_MapTool.SetWindowPos(NULL, 0, 25, rc.Width() - 5, rc.Height() - 5, SWP_SHOWWINDOW);
	m_MapTool.ShowWindow(SW_SHOW);

	m_NaviTool.Create(IDD_NAVITOOL, &m_TabControl );
	m_NaviTool.SetWindowPos(NULL, 0, 25, rc.Width() - 5, rc.Height() - 5, SWP_SHOWWINDOW);
	m_NaviTool.ShowWindow(SW_HIDE);

	Invalidate(FALSE);
}
