#include "pch.h"
#include "SimplePacket.h"
#include "TaskTimerMgr.h"
#include "ClientSession.h"
#include "Horse.h"

//static ServerCore::S_ptr<ClientSession> GetClientSession(const ServerCore::S_ptr<ServerCore::PacketSession>& pSession_)
//{
//	return std::static_pointer_cast<ClientSession>(pSession_);
//}

//extern std::array<std::shared_ptr<ClientSession>, 15000> g_session_arr;

static ClientSession* const GetClientSession(const ServerCore::S_ptr<ServerCore::PacketSession>& pSession_)
{
	return static_cast<ClientSession* const>(pSession_.get());
}
constexpr std::pair<short, short> GetSectorRowCol(const short y_, const short x_)noexcept {
	return { y_ / SECTOR_SIZE,x_ / SECTOR_SIZE };
}

//std::function<bool(ServerCore::Session*, ServerCore::Session*)> g_huristic = [](ServerCore::Session* a, ServerCore::Session* b)
//	{
//		if constexpr (0 == VIEW_RANGE)
//			return true;
//		const auto& aa = static_cast<ClientSession*>(a)->GetHorse();
//		const auto& bb = static_cast<ClientSession*>(b)->GetHorse();
//		const int dist = (aa->m_xPos - bb->m_xPos) * (aa->m_xPos - bb->m_xPos) +
//			(aa->m_yPos - bb->m_yPos) * (aa->m_yPos - bb->m_yPos);
//		return dist <= VIEW_RANGE * VIEW_RANGE;
//	};

namespace ServerCore
{
	const bool CS_MOVE_PACKET::Handle(const S_ptr<PacketSession>& pSession_, const CS_MOVE_PACKET& pkt_)
	{
		const auto pClient = GetClientSession(pSession_);
		const auto pHorse = pClient->GetContentsEntity()->GetHorse();
		pClient->m_lastMoveTime = pkt_.move_time;
		short x = (short)pHorse->m_xPos;
		short y = (short)pHorse->m_yPos;
		const auto [prev_y, prev_x] = GetSectorRowCol(y, x);
		switch (pkt_.direction) {
		case 0: if (y > 0) y--; break;
		case 1: if (y < W_HEIGHT - 1) y++; break;
		case 2: if (x > 0) x--; break;
		case 3: if (x < W_WIDTH - 1) x++; break;
		}
		pHorse->m_xPos = x;
		pHorse->m_yPos = y;

		SC_MOVE_PLAYER_PACKET p;
		p.id = (int)pSession_->GetSessionID();
		p.pkt_size = sizeof(SC_MOVE_PLAYER_PACKET);
		p.pkt_id = SC_MOVE_PLAYER;
		p.x = x;
		p.y = y;
		p.move_time = pClient->m_lastMoveTime;
		const auto rc = GetSectorRowCol(p.y, p.x);
		
		pSession_ << p;
		ServerCore::SC_REMOVE_PLAYER_PACKET remove_packet;
		{
			remove_packet.id = (int)pSession_->GetSessionID();
			remove_packet.pkt_size = sizeof(remove_packet);
			remove_packet.pkt_id = SC_REMOVE_PLAYER;
		}
		SC_ADD_PLAYER_PACKET add_packet;
		{
			add_packet.id = (int)pClient->GetSessionID();
			strcpy_s(add_packet.name, pClient->getName());
			add_packet.pkt_size = sizeof(add_packet);
			add_packet.pkt_id = SC_ADD_PLAYER;
			add_packet.x = (short)pClient->GetContentsEntity()->GetHorse()->m_xPos;
			add_packet.y = (short)pClient->GetContentsEntity()->GetHorse()->m_yPos;
		}
#if defined(NON_BLOCK) || defined(SOCK_TH)
		for (const auto& pSession : g_session_arr)
		{
			if (!pSession)continue;
			pSession << p;
		}
#endif
		//Horse::g_board << p;
#if defined(MULTI_IOCP) || defined(SINGLE_IOCP) || defined(OVERLAPPED_IO)
		//Horse::g_board->BroadCast(p.MakeSendBuffer());
		//const auto cur_room = (Chess*)pSession_->GetCurrentSessionRoomInfo().GetPtr();
		//cur_room->MoveBroadCastEnqueue(pSession_, add_packet.MakeSendBuffer(), remove_packet.MakeSendBuffer(), p.MakeSendBuffer());
		Horse::g_board[rc.first][rc.second]->MoveBroadCastEnqueue(pSession_, add_packet.MakeSendBuffer(),remove_packet.MakeSendBuffer(), p.MakeSendBuffer());
		if (std::pair<short, short>{prev_y, prev_x} != rc)
		{
			//cur_room->ImmigrationEnqueue(Horse::g_board[rc.first][rc.second], pSession_->GetSessionID());
			Horse::g_board[prev_y][prev_x]->ImmigrationEnqueue(Horse::g_board[rc.first][rc.second], pSession_->GetSessionID());
		}
#endif
		return true;
	}
	const bool SC_LOGIN_INFO_PACKET::Handle(const S_ptr<PacketSession>& pSession_, const SC_LOGIN_INFO_PACKET& pkt_)
	{
		return false;
	}
	const bool SC_ADD_PLAYER_PACKET::Handle(const S_ptr<PacketSession>& pSession_, const SC_ADD_PLAYER_PACKET& pkt_)
	{
		return false;
	}
	const bool SC_REMOVE_PLAYER_PACKET::Handle(const S_ptr<PacketSession>& pSession_, const SC_REMOVE_PLAYER_PACKET& pkt_)
	{
		SC_REMOVE_PLAYER_PACKET p;
		p.id = (int)pSession_->GetSessionID();
		p.pkt_size = sizeof(p);
		p.pkt_id = SC_REMOVE_PLAYER;
		
		//Horse::g_board->BroadCast(p.MakeSendBuffer());


		return true;
	}
	const bool SC_MOVE_PLAYER_PACKET::Handle(const S_ptr<PacketSession>& pSession_, const SC_MOVE_PLAYER_PACKET& pkt_)
	{
		return false;
	}
	const bool CS_LOGIN_PACKET::Handle(const S_ptr<PacketSession>& pSession_, const CS_LOGIN_PACKET& pkt_)
	{
		const auto pClient = GetClientSession(pSession_);
		SC_LOGIN_INFO_PACKET p;
		p.id = (int)pSession_->GetSessionID();
		p.pkt_size = sizeof(SC_LOGIN_INFO_PACKET);
		p.pkt_id = SC_LOGIN_INFO;
		p.x = rand() % W_WIDTH;
		p.y = rand() % W_HEIGHT;
		strcpy(pClient->getName(),pkt_.name);

		const auto pHorse = pClient->GetContentsEntity()->GetHorse().get();
		pHorse->m_xPos = p.x;
		pHorse->m_yPos = p.y;
		
		const auto [y, x] = GetSectorRowCol(p.y, p.x);

			//Horse::g_board->EnqueueAsync([pClient]()
			//{
			//	SC_ADD_PLAYER_PACKET add_packet;
			//	add_packet.id = (short)pClient->GetSessionID();
			//	strcpy_s(add_packet.name, pClient->getName());
			//	add_packet.pkt_size = sizeof(add_packet);
			//	add_packet.pkt_id = SC_ADD_PLAYER;
			//	add_packet.x = (short)pClient->GetHorse()->m_xPos;
			//	add_packet.y = (short)pClient->GetHorse()->m_yPos;
			//
			//	for (const auto pSession : Horse::g_board->GetSessionList())
			//	{
			//		pSession << add_packet;
			//
			//		const auto other = static_cast<ClientSession* const>(pSession);
			//
			//		SC_ADD_PLAYER_PACKET add_packet2;
			//		add_packet2.id = (short)other->GetSessionID();
			//		strcpy_s(add_packet.name, other->getName());
			//		add_packet2.pkt_size = sizeof(add_packet);
			//		add_packet2.pkt_id = SC_ADD_PLAYER;
			//		add_packet2.x = (short)other->GetHorse()->m_xPos;
			//		add_packet2.y = (short)other->GetHorse()->m_yPos;
			//
			//		pClient << add_packet2;
			//	}
			//});
			SC_ADD_PLAYER_PACKET add_packet;
			add_packet.id = (int)pClient->GetSessionID();
			strcpy_s(add_packet.name, pClient->getName());
			add_packet.pkt_size = sizeof(add_packet);
			add_packet.pkt_id = SC_ADD_PLAYER;
			add_packet.x = (short)pClient->GetContentsEntity()->GetHorse()->m_xPos;
			add_packet.y = (short)pClient->GetContentsEntity()->GetHorse()->m_yPos;
			SC_MOVE_PLAYER_PACKET p2;
			{
				p2.id = (int)pSession_->GetSessionID();
				p2.pkt_size = sizeof(SC_MOVE_PLAYER_PACKET);
				p2.pkt_id = SC_MOVE_PLAYER;
				p2.x = p.x;
				p2.y = p.y;
				p2.move_time = pClient->m_lastMoveTime;
				const auto rc = GetSectorRowCol(p2.y, p2.x);
			}
			ServerCore::SC_REMOVE_PLAYER_PACKET remove_packet;
			{
				remove_packet.id = (int)pSession_->GetSessionID();
				remove_packet.pkt_size = sizeof(remove_packet);
				remove_packet.pkt_id = SC_REMOVE_PLAYER;
			}

#if defined(NON_BLOCK) || defined(SOCK_TH)
			for (const auto& pSession : g_session_arr)
			{
				if (!pSession)continue;
				pSession << add_packet;

				const auto other = static_cast<ClientSession* const>(pSession.get());

				SC_ADD_PLAYER_PACKET add_packet2;
				add_packet2.id = (short)other->GetSessionID();
				//strcpy_s(add_packet.name, other->getName());
				add_packet2.pkt_size = sizeof(add_packet);
				add_packet2.pkt_id = SC_ADD_PLAYER;
				add_packet2.x = (short)other->GetHorse()->m_xPos;
				add_packet2.y = (short)other->GetHorse()->m_yPos;

				pClient << add_packet2;
			}
#endif
#if defined(MULTI_IOCP) || defined(SINGLE_IOCP) || defined(OVERLAPPED_IO)
			//Horse::g_board[y][x]->EnqueueAsync([add_packet,y,x,pClient]()mutable
			//	{
			//		for (const auto sector : Horse::g_board[y][x]->m_adjVector)
			//		{
			//			if (!sector)continue;
			//			for (const auto pObj : sector->GetObjectList())
			//			{
			//				if (const auto pSession = pObj->IsSession())
			//					pSession << add_packet;
			//
			//				const auto& other_entitiy = pObj->GetContentsEntity();
			//
			//				SC_ADD_PLAYER_PACKET add_packet2;
			//				add_packet2.id = (int)pObj->GetObjectID();
			//				strcpy_s(add_packet.name, other_entitiy->getName());
			//				add_packet2.pkt_size = sizeof(add_packet);
			//				add_packet2.pkt_id = SC_ADD_PLAYER;
			//				add_packet2.x = (short)other_entitiy->GetHorse()->m_xPos;
			//				add_packet2.y = (short)other_entitiy->GetHorse()->m_yPos;
			//
			//				pClient << add_packet2;
			//			}
			//		}
			//	});
			const auto rc = GetSectorRowCol(p.y, p.x);
			//Horse::g_board[y][x]->EnterEnqueue(pSession_);
			Horse::g_board[rc.first][rc.second]->EnqueueAsync([p,y,x,pSession_,add_pkt=add_packet.MakeSendBuffer(),rm_pkt=remove_packet.MakeSendBuffer(),move_pkt=p2.MakeSendBuffer()]()mutable noexcept
				{
					Horse::g_board[y][x]->Enter(pSession_);
					Horse::g_board[y][x]->MoveBroadCastEnqueue(pSession_,std::move(add_pkt), std::move(rm_pkt), std::move(move_pkt));
					pSession_ << p;
				});
#endif
		return true;
	}
}
