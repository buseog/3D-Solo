#pragma once


// CCameraTool ��ȭ �����Դϴ�.

class CCameraTool : public CDialog
{
	DECLARE_DYNAMIC(CCameraTool)

public:
	CCameraTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CCameraTool();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CAMERATOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
