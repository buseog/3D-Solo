USING(Engine)

CManagement* Get_Management(void)
{
	return CManagement::GetInstance();
}

CComponentMgr*	Get_ComponentMgr(void)
{
	return CComponentMgr::GetInstance();
}

CCameraMgr*		Get_CameraMgr(void)
{
	return CCameraMgr::GetInstance();
}

CNaviMgr*		Get_NaviMgr(void)
{
	return CNaviMgr::GetInstance();
}

void Release_Utility(void)
{
	if (CNaviMgr::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CNaviMgr Release Failed");

	if (CCameraMgr::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CCameraMgr Release Failed");

	if (CManagement::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CManagement Release Failed");

	if (CComponentMgr::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CComponentMgr Release Failed");
}