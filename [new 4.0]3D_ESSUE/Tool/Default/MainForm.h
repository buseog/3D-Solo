#pragma once
#include "afxcmn.h"

#include "MapTool.h"
#include "NaviTool.h"

// CMainForm �� ���Դϴ�.

class CMainForm : public CFormView
{
	DECLARE_DYNCREATE(CMainForm)

public:
	_uint		m_iToolType;
	CMapTool	m_MapTool;
	CNaviTool	m_NaviTool;

protected:
	CMainForm();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CMainForm();

public:
	enum { IDD = IDD_MAINFORM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabControl;
	afx_msg void OnChangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnInitialUpdate();
};


