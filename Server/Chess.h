#pragma once
#include "pch.h"


class Chess
{
public:
	Chess();
	~Chess();
public:
	Vec2 GetBoardPos(const int y_, const int x_)const { return m_chessBoard[y_][x_]; }
private:
	Vec2 m_chessBoard[8][8];
};

