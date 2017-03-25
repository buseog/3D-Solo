#include "InputDevice.h"

USING(Engine)
IMPLEMENT_SINGLETON(CInputDevice)

Engine::CInputDevice::CInputDevice(void)
: m_pInputSDK(NULL)
, m_pKeyBoard(NULL)
, m_pMouse(NULL)
{

}

Engine::CInputDevice::~CInputDevice(void)
{

}

HRESULT Engine::CInputDevice::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// ��Ű���带 �����ϱ����� ������ü�� �����۳�.
	if(FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, NULL)))
		return E_FAIL;	

	// �� ��ǻ�Ϳ� ����Ǿ��ִ� Ű���尴ü�� �����ϳ�.
	if(FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, NULL)))
		return E_FAIL;

	// �������� Ű���� ��ü�� ���� ������ �����Ѵ�.
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ������ Ű������ ���� ������ �����ϳ�.
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ������ Ű���带 Ȱ��ȭ��Ų��.
	m_pKeyBoard->Acquire();

	// �� ��ǻ�Ϳ� ����Ǿ��ִ� Ű���尴ü�� �����ϳ�.
	if(FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, NULL)))
		return E_FAIL;

	// �������� Ű���� ��ü�� ���� ������ �����Ѵ�.
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ������ Ű������ ���� ������ �����ϳ�.
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ������ Ű���带 Ȱ��ȭ��Ų��.
	m_pMouse->Acquire();

	return S_OK;
}

void Engine::CInputDevice::Update(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

_byte Engine::CInputDevice::GetDIKeyState(_ubyte byKeyID)
{
	return m_byKeyState[byKeyID];
}

_byte Engine::CInputDevice::GetDIMouseState(MOUSEKEYSTATE byMouseID)
{
	return m_MouseState.rgbButtons[byMouseID];
}

_long Engine::CInputDevice::GetDIMouseMove(MOUSEMOVESTATE byMouseMoveState)
{
	return *(((_long*)(&m_MouseState)) + byMouseMoveState);
}

void Engine::CInputDevice::Free(void)
{
	Engine::Safe_Release(m_pKeyBoard);
	Engine::Safe_Release(m_pMouse);
	Engine::Safe_Release(m_pInputSDK);
}

