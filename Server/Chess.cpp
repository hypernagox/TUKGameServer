#include "pch.h"
#include "Chess.h"

Chess::Chess()
	:SessionManageable{ 0 }
{
	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			m_chessBoard[y][x] = Vec2{ 70.f,65.f } + Vec2{ 95.f,0 } *(float)x + Vec2{ 0,95.f }*(float)y;
		}
	}
}

Chess::~Chess()
{
}
