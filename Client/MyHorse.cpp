#include "pch.h"
#include "MyHorse.h"
#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "Chess.h"

MyHorse::MyHorse()
{
}

MyHorse::~MyHorse()
{
}

void MyHorse::update()
{
	if (KEY_TAP(VK_LEFT))
	{
		m_xPos = wrapAround(m_xPos - 1, 0, 8);
	}
	if (KEY_TAP(VK_DOWN))
	{
		m_yPos = wrapAround(m_yPos + 1, 0, 8);
	}
	if (KEY_TAP(VK_RIGHT))
	{
		m_xPos = wrapAround(m_xPos + 1, 0, 8);
	}
	if (KEY_TAP(VK_UP))
	{
		m_yPos = wrapAround(m_yPos - 1, 0, 8);
	}

	SetPos(m_pCurBoard->GetBoardPos(m_yPos, m_xPos));
}

void MyHorse::component_update()
{
}
