#pragma once
#include "pch.h"
#include "CObject.h"

class Chess;

class Horse
	:public CObject
{
public:
	Horse();
	~Horse();
public:
	void SetImg(CImage* const _pImg) { m_pImg = _pImg; }
	const CImage* GetImg()const { return m_pImg; }
	Horse* Clone()const override
	{
		auto p = new Horse{ *this };
		return p;
	}
	void render(HDC _dc)const override;
	void SetBoard(Chess* pBoard_) { m_pCurBoard = pBoard_; }
	Chess* GetBoard()const { return m_pCurBoard; }
	void SetHorsePos(const int y_, const int x_,Vec2 pos_);
protected:
	int m_xPos = 0;
	int m_yPos = 0;
	Chess* m_pCurBoard;
private:
	const CImage* m_pImg;
};

