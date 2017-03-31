#pragma once

#include "Export_Engine.h"
#include "Include.h"
#include "afxwin.h"
// CNaviTool ��ȭ �����Դϴ�.

class CNaviTool : public CDialog
{
	DECLARE_DYNAMIC(CNaviTool)
public:
	typedef struct SaveNavi
	{
		MyVec3 vPointA;
		MyVec3 vPointB;
		MyVec3 vPointC;
	}SaveNavi;

public:
	_int	TriggerType;
	_int	NotifyType;

	float MouseX;
	float MouseY;
	float MouseZ;

	float PosX;
	float PosY;
	float PosZ;

	MyVec3 vSelCube[3];
	_int	iSelCnt;
	_int	SelNaviIndex;

public:
	CNaviTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CNaviTool();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_NAVITOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CListBox NaviList;
	void SetMousePos(POINT pt);
	void LinkCell(const MyVec3* pPos);

	afx_msg void OnDelete();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	CButton iTrigger[3];
	afx_msg void OnNaviList();
	virtual BOOL OnInitDialog();
};
