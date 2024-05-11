#include "pch.h"
#include "NPC.h"
#include "ContentsEntity.h"
#include "Chess.h"
#include "Horse.h"
#include "SimplePacket.h"

constexpr std::pair<short, short> GetSectorRowCol(const short y_, const short x_)noexcept {
	return { y_ / SECTOR_SIZE,x_ / SECTOR_SIZE };
}


NPC::NPC()
	:ServerCore::TimerObject{ 1,ServerCore::MakeShared<ContentsEntity>() }
{
}

NPC::~NPC()
{
	std::cout << "Destroy NPC!!" << std::endl;
}

const ServerCore::TIMER_STATE NPC::TimerUpdate() noexcept
{
	
	//const auto e = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_time).count();
	//if (e >= 1500) {
	//
	//	std::cout << "ms - " << e << '\n';
	//}
	const auto& pHorse = GetContentsEntity()->GetHorse();
	auto& x = pHorse->m_xPos;
	auto& y = pHorse->m_yPos;
	const auto [prev_y, prev_x] = GetSectorRowCol(y, x);
	switch (rand() % 4)
	{
	case 0:if (x > 0)x--; break;
	case 1:if (x < W_WIDTH - 1)x++; break;
	case 2:if (y > 0)y--; break;
	case 3:if (y < W_HEIGHT - 1)y++; break;
	}
	
	ServerCore::SC_MOVE_PLAYER_PACKET p;
	p.id = (int)GetObjectID();
	p.pkt_size = sizeof(ServerCore::SC_MOVE_PLAYER_PACKET);
	p.pkt_id = SC_MOVE_PLAYER;
	p.x = x;
	p.y = y;
	
	const auto rc = GetSectorRowCol(p.y, p.x);

	
	ServerCore::SC_REMOVE_PLAYER_PACKET remove_packet;
	{
		remove_packet.id = (int)GetObjectID();
		remove_packet.pkt_size = sizeof(remove_packet);
		remove_packet.pkt_id = SC_REMOVE_PLAYER;
	}
	ServerCore::SC_ADD_PLAYER_PACKET add_packet;
	{
		add_packet.id = (int)GetObjectID();
		strcpy_s(add_packet.name,GetContentsEntity()->getName());
		add_packet.pkt_size = sizeof(add_packet);
		add_packet.pkt_id = SC_ADD_PLAYER;
		add_packet.x = (short)GetContentsEntity()->GetHorse()->m_xPos;
		add_packet.y = (short)GetContentsEntity()->GetHorse()->m_yPos;
	}

	const int sector_state = Horse::g_board[rc.first][rc.second]->MoveBroadCastEnqueue(SharedCastThis<ServerCore::IocpEntity>(),
		add_packet.MakeSendBuffer()
		, remove_packet.MakeSendBuffer(),
		p.MakeSendBuffer());
	const auto exec_time = std::chrono::steady_clock::now() - m_time;
	if constexpr (true == POSSIBILITY_ON)
	{
		const auto r = rand() % 100;
		if (r < POSSIBILITY)
			std::cout << GetContentsEntity()->getName() << " : " << std::chrono::duration_cast<std::chrono::milliseconds>(exec_time).count() << "ms\n";
	}
	if (std::pair<short, short>{prev_y, prev_x} != rc)
	{
		Horse::g_board[prev_y][prev_x]->ImmigrationEnqueue(Horse::g_board[rc.first][rc.second], GetObjectID());
	}
	//return ServerCore::TIMER_STATE::RUN;
	m_time = std::chrono::steady_clock::now();
	if (ServerCore::SECTOR_STATE::USER_EMPTY & sector_state)
	{
		return ServerCore::TIMER_STATE::IDLE;
	}
	else
	{
		return ServerCore::TIMER_STATE::RUN;
	}
}
