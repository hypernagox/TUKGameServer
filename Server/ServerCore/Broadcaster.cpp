#include "ServerCorePch.h"
#include "Broadcaster.h"
#include "PacketSession.h"
#include "../SimplePacket.h"
#include "../ClientSession.h"
#include "../Horse.h"

extern std::function<bool(ServerCore::Session*, ServerCore::Session*)> g_huristic;

namespace ServerCore
{
	Broadcaster::Broadcaster()
	{
	}

	Broadcaster::~Broadcaster()
	{
	}

	void Broadcaster::BroadcastMove(
		const S_ptr<SendBuffer>& in_pkt,
		const S_ptr<SendBuffer>& out_pkt,
		const S_ptr<SendBuffer>& move_pkt,
		const S_ptr<Session>& thisSession,
		const Vector<SessionManageable*>& sectors
	)noexcept
	{
		//thread_local HashSet<S_ptr<PacketSession>> new_view_list;
		
		new_view_list.clear();

		//session_list_lock.lock_shared();
		for (const auto sector : sectors)
		{
			sector->m_srwLock.lock_shared();
			for (const auto pSession : sector->GetSessionList())
			{
				if (thisSession.get() == pSession)continue;
				if (pSession->IsConnected())
				{
					if (g_huristic(thisSession.get(), pSession)) 
					{
						new_view_list.emplace(pSession->SharedCastThis<PacketSession>());
					}
				}
			}
			sector->m_srwLock.unlock_shared();
		}
		//session_list_lock.unlock_shared();
		//std::ranges::sort(new_view_list, {}, &Session::GetSessionID);
		//auto& m_viewList = m_viewList2[Mgr(ThreadMgr)->GetCurThreadIdx()];
		//ServerCore::SC_REMOVE_PLAYER_PACKET remove_packet;
		//{
		//	remove_packet.id = (short)pSession_->GetSessionID();
		//	remove_packet.pkt_size = sizeof(remove_packet);
		//	remove_packet.pkt_id = SC_REMOVE_PLAYER;
		//}
		//SC_ADD_PLAYER_PACKET add_packet;
		//{
		//	add_packet.id = (short)pClient->GetSessionID();
		//	strcpy_s(add_packet.name, pClient->getName());
		//	add_packet.pkt_size = sizeof(add_packet);
		//	add_packet.pkt_id = SC_ADD_PLAYER;
		//	add_packet.x = (short)pClient->GetHorse()->m_xPos;
		//	add_packet.y = (short)pClient->GetHorse()->m_yPos;
		//}
		//m_spinLock.lock();
		for (const auto& pSession : new_view_list)
		{
			//const auto iter = std::ranges::lower_bound(m_viewList, pSession->GetSessionID(), {}, &Session::GetSessionID);
			if (!m_viewList.contains(pSession))
			{
				// 추가하고 입장패킷을 보낸다
				//thisSession->SendAsync(in_pkt);
				pSession->SendAsync(in_pkt);
				SC_ADD_PLAYER_PACKET add_packet;
				{
					const auto pClient = static_cast<ClientSession*>(pSession.get());
					add_packet.id = (short)pSession->GetSessionID();
					strcpy_s(add_packet.name, pClient->getName());
					add_packet.pkt_size = sizeof(add_packet);
					add_packet.pkt_id = SC_ADD_PLAYER;
					add_packet.x = (short)pClient->GetHorse()->m_xPos;
					add_packet.y = (short)pClient->GetHorse()->m_yPos;
				}
				thisSession->SendAsync(add_packet.MakeSendBuffer());
				//thisSession->SendAsync(move_pkt);
				//pSession->SendAsync(move_pkt);
				//m_viewList.emplace(static_cast<PacketSession* const>(pSession));
				m_viewList.emplace(pSession);
			}
			else
			{
				// 이동패킷을 보내면 된다
				thisSession->SendAsync(move_pkt);
				pSession->SendAsync(move_pkt);
			}
		}

		//m_viewList.sort([](const Session* const a, const Session* const b)noexcept {return *a < *b; });
		const auto e_iter = new_view_list.cend();
		for (auto iter = m_viewList.cbegin(); iter != m_viewList.cend();)
		{
			const auto& pSession = *iter;
			//const auto target = std::ranges::lower_bound(new_view_list, pSession->GetSessionID(), {}, &Session::GetSessionID);
			const auto target = new_view_list.find(pSession);
			if (e_iter == target)
			{
				// 없앤다
				//thisSession->SendAsync(out_pkt);
				pSession->SendAsync(out_pkt);
				ServerCore::SC_REMOVE_PLAYER_PACKET remove_packet;
				{
					
					remove_packet.id = (short)pSession->GetSessionID();
					remove_packet.pkt_size = sizeof(remove_packet);
					remove_packet.pkt_id = SC_REMOVE_PLAYER;
				}
				thisSession->SendAsync(remove_packet.MakeSendBuffer());
				iter = m_viewList.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		//m_spinLock.unlock();
	}
}