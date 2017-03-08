#pragma once


// CCameraTool 대화 상자입니다.

class CCameraTool : public CDialog
{
	DECLARE_DYNAMIC(CCameraTool)

public:
	CCameraTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCameraTool();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CAMERATOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
