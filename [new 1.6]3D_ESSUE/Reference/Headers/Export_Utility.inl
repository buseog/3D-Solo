USING(Engine)

CManagement* Get_Management(void)
{
	return CManagement::GetInstance();
}

CComponentMgr*	Get_ComponentMgr(void)
{
	return CComponentMgr::GetInstance();
}

void Release_Utility(void)
{
	if (CManagement::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CManagement Release Failed");

	if (CComponentMgr::GetInstance()->DestroyInstance() != 0)
		MSG_BOX("CComponentMgr Release Failed");
}