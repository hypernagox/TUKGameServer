#include "pch.h"
#include "Horse.h"
#include "CResMgr.h"
#include "CKeyMgr.h"

Horse::Horse()
{
	SetScale({ 64,64 });
}

Horse::~Horse()
{
}

void Horse::render(HDC _dc) const
{
	Mgr(CResMgr)->renderImg(_dc, m_pImg, this, { 0,0 }, { 62,62 }, false, RGB(255, 255, 255));
}
