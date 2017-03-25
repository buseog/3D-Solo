#ifndef InputDevice_h__
#define InputDevice_h__

#include "Engine_BigHeaders.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInputDevice :public CBase
{
	DECLARE_SINGLETON(CInputDevice)

public:
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_HB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

private:
	LPDIRECTINPUT8				m_pInputSDK;

private:
	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;

private:
	_byte						m_byKeyState[256];
	DIMOUSESTATE				m_MouseState;

public:
	_byte GetDIKeyState(_ubyte byKeyID);
	_byte GetDIMouseState(MOUSEKEYSTATE byMouseID);
	_long GetDIMouseMove(MOUSEMOVESTATE byMouseMoveState);

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

public:
	virtual void Free(void);

private:
	explicit CInputDevice(void);
	virtual ~CInputDevice(void);
};

END
#endif // InputDevice_h__