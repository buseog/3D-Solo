#ifndef Export_System_h__
#define Export_System_h__

#include "Engine_BigHeaders.h"
#include "TimerMgr.h"
#include "FrameMgr.h"
#include "FontMgr.h"
#include "GraphicDev.h"
#include "InputDevice.h"
#include "Keymgr.h"

BEGIN(Engine)

inline CGraphicDev* Get_GraphicDev(void);
inline CTimerMgr*	Get_TimeMgr(void);
inline CFrameMgr*	Get_FrameMgr(void);
inline CFontMgr*	Get_FontMgr(void);
inline CInputDevice* Get_InputDevice(void);

inline _byte GetDIKeyState(_ubyte byKeyID);
inline _byte GetDIMouseState(CInputDevice::MOUSEKEYSTATE byMouseID);
inline _long GetDIMouseMove(CInputDevice::MOUSEMOVESTATE byMouseMoveState);

//// Release
inline void Release_System(void);

#include "Export_System.inl"

END
#endif // Export_System_h__
