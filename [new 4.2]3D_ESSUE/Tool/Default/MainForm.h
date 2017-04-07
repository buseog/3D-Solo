#pragma once
#include "afxcmn.h"

#include "MapTool.h"
#include "NaviTool.h"

// CMainForm 폼 뷰입니다.

class CMainForm : public CFormView
{
	DECLARE_DYNCREATE(CMainForm)

public:
	_uint		m_iToolType;
	CMapTool	m_MapTool;
	CNaviTool	m_NaviTool;

protected:
	CMainForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabControl;
	afx_msg void OnChangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnInitialUpdate();
};


