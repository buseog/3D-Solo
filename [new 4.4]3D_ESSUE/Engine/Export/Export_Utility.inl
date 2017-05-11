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

CTargetMgr*		Get_TargetMgr(void)
{
	return CTargetMgr::GetInstance();
}

CLightMgr*		Get_LightMgr(void)
{
	return CLightMgr::GetInstance();
}

void Release_Utility(void)
{
	if (CLightMgr::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CLightMgr Release Failed");

	if (CTargetMgr::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CTargetMgr Release Failed");

	if (CNaviMgr::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CNaviMgr Release Failed");

	if (CCameraMgr::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CCameraMgr Release Failed");

	if (CManagement::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CManagement Release Failed");

	if (CComponentMgr::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CComponentMgr Release Failed");
}