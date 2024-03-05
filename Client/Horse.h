#pragma once
#include "pch.h"
#include "CObject.h"

class Horse
	:public CObject
{
public:
	void SetImg(CImage* const _pImg) { m_pImg = _pImg; }
	const CImage* GetImg()const { return m_pImg; }
	Horse* Clone()const override
	{
		auto p = new Horse{ *this };
		return p;
	}

	void render(HDC _dc)const override;
private:
	const CImage* m_pImg;
};

