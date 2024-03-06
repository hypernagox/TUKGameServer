#pragma once
#include "pch.h"
#include "CScene.h"
#include "CLayer.h"

class Chess
	:public CScene
{
public:
	Chess();
	~Chess();
public:
	Vec2 GetBoardPos(const int y_, const int x_)const { return m_chessBoard[y_][x_]; }
	void Enter()override;
	void render(HDC _dc)override;
private:
	vector<unique_ptr<CLayer>> m_vecLayer;
	Vec2 m_chessBoard[8][8];
};

