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
	void Enter()override;
	void render(HDC _dc)override;
	void AddNewPlayer(const uint64_t newPlayerID_,const int y,const int x,const Vec2 pos);
	const auto& GetOtherPlayerMap()const noexcept { return m_mapOtherPlayers; }
	void LeavePlayer(const uint64_t leavePlayerID);
private:
	vector<unique_ptr<CLayer>> m_vecLayer;
	unordered_map<uint64_t, class Horse*> m_mapOtherPlayers;
};

