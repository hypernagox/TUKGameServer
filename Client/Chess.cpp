#include "pch.h"
#include "Chess.h"
#include "CResMgr.h"
#include "MyHorse.h"
#include "NetworkMgr.h"
#include "ServerSession.h"

static std::shared_ptr<ServerSession> GetServerSession()
{
	return std::static_pointer_cast<ServerSession>(NetMgr(NetworkMgr)->GetSession());
}

Chess::Chess()
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

void Chess::Enter()
{
	const auto pHorse = new MyHorse;
	const auto pImg = Mgr(CResMgr)->GetImg(L"black_horse.png");
	pImg->SetTransparentColor(RGB(255, 255, 255));
	pHorse->SetImg(pImg);
	GetServerSession()->SetHorse(pHorse);
	

	pHorse->SetBoard(this);
	AddObject(pHorse,GROUP_TYPE::PLAYER);

	const auto pLayer = CLayer::CreateLayer(L"chess_map.png", { 0,0 }, { 800,800 }, { 800,800 }, 1, 1.f);
	
	m_vecLayer.emplace_back(pLayer);
}

void Chess::render(HDC _dc)
{
	for (const auto& pLayer : m_vecLayer)
	{
		pLayer->render(_dc);
	}
	CScene::render(_dc);
}
