USING(Engine)

CGraphicDev* Get_GraphicDev(void)
{
	return CGraphicDev::GetInstance();
}

CTimerMgr*	Get_TimeMgr(void)
{
	return CTimerMgr::GetInstance();
}

CFrameMgr*	Get_FrameMgr(void)
{
	return CFrameMgr::GetInstance();
}

CFontMgr*	Get_FontMgr(void)
{
	return CFontMgr::GetInstance();
}

CInputDevice* Get_InputDevice(void)
{
	return CInputDevice::GetInstance();
}

_byte Engine::GetDIKeyState(_ubyte byKeyID)
{
	return CInputDevice::GetInstance()->GetDIKeyState(byKeyID);
}
_byte Engine::GetDIMouseState(CInputDevice::MOUSEKEYSTATE byMouseID)
{
	return CInputDevice::GetInstance()->GetDIMouseState(byMouseID);
}
_long Engine::GetDIMouseMove(CInputDevice::MOUSEMOVESTATE byMouseMoveState)
{
	return CInputDevice::GetInstance()->GetDIMouseMove(byMouseMoveState);
}

void Engine::Release_System(void)
{
	if (CTimerMgr::GetInstance()->DestroyInstance())
		MSG_BOX("TimerMgr Release Failed");

	if (CFrameMgr::GetInstance()->DestroyInstance())
		MSG_BOX("FrameMgr Release Failed");

	if (CFontMgr::GetInstance()->DestroyInstance())
		MSG_BOX("CFontMgr Release Failed");

	if (CInputDevice::GetInstance()->DestroyInstance())
		MSG_BOX("CInputDevice Release Failed");

	if (CGraphicDev::GetInstance()->DestroyInstance())
		MSG_BOX("CGraphicDev Release Failed");

}