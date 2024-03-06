#include "pch.h"
#include "CKeyMgr.h"
#include "CCore.h"	


extern HHOOK hHook;

CKeyMgr::CKeyMgr()
{

}

CKeyMgr::~CKeyMgr() 
{

}

void CKeyMgr::init()
{
	m_mapKey.reserve(128);
}

void CKeyMgr::update()
{
	const HWND hWnd{ GetFocus() };	

	if (hWnd)		
	{
		for (auto& [VKval, keyInfo] : m_mapKey)
		{
			if (GetAsyncKeyState(VKval) & 0x8000)
			{
				if (keyInfo.bPrevPush)
				{
					keyInfo.eState = KEY_STATE::HOLD;
				}
				else
				{
					keyInfo.eState = KEY_STATE::TAP;
				}
				keyInfo.bPrevPush = true;
			}
			else
			{
				if (keyInfo.bPrevPush)
				{
					keyInfo.eState = KEY_STATE::AWAY;
				}
				else
				{
					keyInfo.eState = KEY_STATE::NONE;
				}
				keyInfo.bPrevPush = false;
			}
		}
		
		GetCursorPos(&m_ptCurMousePos);	
		ScreenToClient(CCore::GetInst()->GetMainHwnd(), &m_ptCurMousePos);
		m_vCurMousePos = m_ptCurMousePos;
	}
	else
	{
		for (auto& [VKval, keyInfo] : m_mapKey)
		{
			keyInfo.bPrevPush = false;

			if (KEY_STATE::TAP == keyInfo.eState || KEY_STATE::HOLD == keyInfo.eState)
			{
				keyInfo.eState = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::AWAY == keyInfo.eState)
			{
				keyInfo.eState = KEY_STATE::NONE;
			}
		}
	}

}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (wParam == WM_MOUSEWHEEL)
		{
			UnhookWindowsHookEx(hHook);
			MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
			Mgr(CKeyMgr)->m_wheelDelta = GET_WHEEL_DELTA_WPARAM(pMouseStruct->mouseData);
			hHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}