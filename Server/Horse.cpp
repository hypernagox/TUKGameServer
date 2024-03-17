#include "pch.h"
#include "Horse.h"

Horse::Horse()
{
	m_vPos = g_board.GetBoardPos(m_yPos, m_xPos);
}

std::pair<POINT, Vec2> Horse::UpdateHorsePos(const int VK)
{
	if (VK_LEFT == VK)
	{
		m_xPos = wrapAround(m_xPos - 1, 0, 8);
	}
	else if (VK_DOWN == VK)
	{
		m_yPos = wrapAround(m_yPos + 1, 0, 8);

	}
	else if (VK_RIGHT == VK)
	{
		m_xPos = wrapAround(m_xPos + 1, 0, 8);
	}
	else if (VK_UP == VK)
	{
		m_yPos = wrapAround(m_yPos - 1, 0, 8);
	}
	m_vPos = g_board.GetBoardPos(m_yPos, m_xPos);
	return { { m_xPos,m_yPos },m_vPos };
}
