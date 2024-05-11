#include "pch.h"
#include "Chess.h"
#include "PacketSession.h"

Chess::Chess()
	:SessionManageable{ 0 }
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

const int Chess::MoveBroadCastEnqueue(
	const ServerCore::S_ptr<ServerCore::IocpEntity>& move_session,
	const ServerCore::S_ptr<ServerCore::SendBuffer>& in_pkt,
	const ServerCore::S_ptr<ServerCore::SendBuffer>& out_pkt,
	const ServerCore::S_ptr<ServerCore::SendBuffer>& move_pkt) noexcept
{
	//move_session->m_broadCaster.m_spinLock.lock();
	//auto temp = ServerCore::MakeUnique<ServerCore::Vector<ServerCore::SessionManageable*>>();
	//temp->reserve(9);
	////SessionManageable::MoveBroadCastEnqueue(move_session, in_pkt, out_pkt, move_pkt);
	//temp->emplace_back(this);
	//for (auto& i : m_adjSector)
	//{
	//	if (!i)continue;
	//	//i->SessionManageable::MoveBroadCastEnqueue(move_session, in_pkt, out_pkt, move_pkt);
	//	temp->emplace_back(i);
	//}
	//SessionManageable::MoveBroadCastEnqueue(move_session, in_pkt, out_pkt, move_pkt, std::move(temp));
	//move_session->m_broadCaster.m_spinLock.unlock();
	//thread_local const auto th_adj_sector = InitAdjSector();
	if constexpr (400 == SECTOR_SIZE)
	{
		if constexpr (0 == VIEW_RANGE)
			SessionManageable::BroadCast(move_pkt);
		else
			return SessionManageable::MoveBroadCast(move_session, in_pkt, out_pkt, move_pkt, &m_adjVector);
		
	}
	else
	{
		//SessionManageable::MoveBroadCast(move_session, in_pkt, out_pkt, move_pkt, &m_adjVector);
		return SessionManageable::MoveBroadCastEnqueue(move_session, in_pkt, out_pkt, move_pkt, &m_adjVector);
	}
}

ServerCore::Vector <ServerCore::SessionManageable*> Chess::InitAdjSector() noexcept
{
	ServerCore::Vector <ServerCore::SessionManageable*> temp;
	temp.reserve(9);
	temp.emplace_back(this);
	for (auto& i : m_adjSector)
	{
		if (!i)continue;
		temp.emplace_back(i);
	}
	return temp;
}
