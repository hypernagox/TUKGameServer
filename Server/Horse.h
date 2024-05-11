#pragma once
#include "Chess.h"

class Horse
{
public:
	Horse();
	~Horse();
	std::pair<POINT,Vec2> UpdateHorsePos(const int VK);
	//static Vec2 GetInitPos() { return g_board->GetBoardPos(0, 0); }
public:
	int m_xPos = 0;
	int m_yPos = 0;
	Vec2 m_vPos = {};
public:
	class ClientSession* m_pSession = nullptr;
	static inline std::shared_ptr<Chess> g_board[2000 / SECTOR_SIZE][2000 / SECTOR_SIZE];
};

