#pragma once


// CMapTool ��ȭ �����Դϴ�.
#include "Export_Engine.h"
#include "Include.h"
#include "afxwin.h"
#include "Obstacle.h"

class CMapTool : public CDialog
{
	DECLARE_DYNAMIC(CMapTool)
public:
	typedef struct SaveObstacle
	{
		TCHAR szKey[256];
		MyVec3 vScale;
		MyVec3 vAngle;
		MyVec3 vPos;
	}SaveObstacle;

public:
	_int	TriggerType;
	_int	NotifyType;
	_int	NotifyOptionType;
	_int	SelObstacleInfo;
	_int	SelObstacleList;
	vector<_tchar*> vecMeshKey;

public:
	CMapTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMapTool();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MAPTOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	float MouseX;
	float MouseY;
	float MouseZ;
	float ScaleX;
	float ScaleY;
	float ScaleZ;
	float AngleX;
	float AngleY;
	float AngleZ;
	float PosX;
	float PosY;
	float PosZ;

	CListBox ObstacleInfo;
	CListBox ObstacleList;
	afx_msg void OnObstacleSave();
	afx_msg void OnObstacleLoad();
	afx_msg void OnObstacleInfo();
	afx_msg void OnObStacleList();
	afx_msg void OnObstacleDelete();
	afx_msg void OnDropFiles(HDROP hDropInfo);

	void SetMousePos(POINT pt);
	void AddObstacleList(CObstacle* pObstacle);
};
