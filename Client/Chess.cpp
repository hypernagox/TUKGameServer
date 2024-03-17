#include "pch.h"
#include "Chess.h"
#include "CResMgr.h"
#include "MyHorse.h"
#include "NetworkMgr.h"
#include "ServerSession.h"
#include "CEventMgr.h"

static std::shared_ptr<ServerSession> GetServerSession()
{
	return std::static_pointer_cast<ServerSession>(NetMgr(NetworkMgr)->GetSession());
}

Chess::Chess()
{

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

void Chess::AddNewPlayer(const uint64_t newPlayerID_, const int y, const int x, const Vec2 pos)
{
	const auto pHorse = new Horse;
	const auto pImg = Mgr(CResMgr)->GetImg(L"white_horse.png");
	pImg->SetTransparentColor(RGB(255, 255, 255));
	pHorse->SetImg(pImg);
	
	pHorse->SetBoard(this);

	pHorse->SetHorsePos(y, x, pos);

	AddObject(pHorse, GROUP_TYPE::PLAYER);
	m_mapOtherPlayers.emplace(newPlayerID_, pHorse);
}

void Chess::LeavePlayer(const uint64_t leavePlayerID)
{
	const auto iter = m_mapOtherPlayers.find(leavePlayerID);
	if (m_mapOtherPlayers.end() != iter)
	{
		DeleteObj(iter->second);
		m_mapOtherPlayers.erase(iter);
	}
}
