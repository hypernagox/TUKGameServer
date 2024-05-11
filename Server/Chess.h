#pragma once
#include "pch.h"
#include "SessionManageable.h"

class ContentsEntity;

//constexpr static int SECTOR_SIZE = 400;	// ���� X
constexpr static int SECTOR_SIZE = 8; // ����ũ�� 8x8
constexpr const int VIEW_RANGE = 11;
//constexpr const int VIEW_RANGE = 0;	// 0���� �ϸ� �þ�ó�� X�� ���� (�Լ���ü�� constexpr�� ����Ͽ����ϴ�.)
constexpr const int NPC_RANGE = VIEW_RANGE - 5;	// �÷��̾ NPC�� ����� ��Ÿ�
constexpr const int POSSIBILITY = 5; // NPC ������ ��¸޼��� Ȯ�� (�÷��̾ ó�� �����ٸ� �׶��� �ٷ� �۵��ϱ� ������ 0ms�� ���� �� �ֽ��ϴ�.)
constexpr const bool POSSIBILITY_ON = true;	// false�� �ٲٸ� ������� ����

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

