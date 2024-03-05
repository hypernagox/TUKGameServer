#include "Chess.h"
#include "Horse.h"
#include "CResMgr.h"
#include "pch.h"

void Chess::Enter()
{
	const auto pHorse = new Horse;
	pHorse->SetImg(Mgr(CResMgr)->GetImg(L"Bullet1.bmp"));
	
	AddObject(pHorse,GROUP_TYPE::PLAYER);
}
