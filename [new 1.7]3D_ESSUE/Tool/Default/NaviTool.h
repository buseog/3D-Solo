#pragma once


// CNaviTool ��ȭ �����Դϴ�.

class CNaviTool : public CDialog
{
	DECLARE_DYNAMIC(CNaviTool)

public:
	CNaviTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CNaviTool();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_NAVITOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
