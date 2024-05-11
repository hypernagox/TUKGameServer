#pragma once
#include "pch.h"
#include "SessionManageable.h"

class ContentsEntity;

//constexpr static int SECTOR_SIZE = 400;	// 섹터 X
constexpr static int SECTOR_SIZE = 8; // 섹터크기 8x8
constexpr const int VIEW_RANGE = 11;
//constexpr const int VIEW_RANGE = 0;	// 0으로 하면 시야처리 X와 같음 (함수자체에 constexpr을 사용하였습니다.)
constexpr const int NPC_RANGE = VIEW_RANGE - 5;	// 플레이어가 NPC를 깨우는 사거리
constexpr const int POSSIBILITY = 5; // NPC 딜레이 출력메세지 확률 (플레이어가 처음 깨웠다면 그때는 바로 작동하기 때문에 0ms가 나올 수 있습니다.)
constexpr const bool POSSIBILITY_ON = true;	// false로 바꾸면 출력하지 않음

class Chess
	:public ServerCore::SessionManageable
{
public:
	Chess();
	~Chess();
public:
	Vec2 GetBoardPos(const int y_, const int x_)const { return m_chessBoard[y_][x_]; }
	virtual void ImigrationAfterBehavior(const ServerCore::S_ptr<ServerCore::SessionManageable> beforeRoom, const ServerCore::S_ptr<ServerCore::IocpEntity> pSession_)noexcept override {};
	const int MoveBroadCastEnqueue(
		const ServerCore::S_ptr<ServerCore::IocpEntity>& move_session,
		const ServerCore::S_ptr<ServerCore::SendBuffer>& in_pkt,
		const ServerCore::S_ptr<ServerCore::SendBuffer>& out_pkt,
		const ServerCore::S_ptr<ServerCore::SendBuffer>& move_pkt) noexcept;
	
public:
	Chess* m_adjSector[8]{};
	ServerCore::Vector <ServerCore::SessionManageable*> m_adjVector;
	ServerCore::LinkedHashMap<uint64, ServerCore::S_ptr<ContentsEntity>> m_objects;
private:
	ServerCore::Vector <ServerCore::SessionManageable*> InitAdjSector()noexcept;
	Vec2 m_chessBoard[8][8];

	

};

