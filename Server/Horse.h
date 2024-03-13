#pragma once
#include "Chess.h"

class Horse
{
public:
	std::pair<POINT,Vec2> UpdateHorsePos(const int VK);
	static Vec2 GetInitPos() { return g_board.GetBoardPos(0, 0); }
private:
	int m_xPos = 0;
	int m_yPos = 0;


	static inline const Chess g_board;
};

