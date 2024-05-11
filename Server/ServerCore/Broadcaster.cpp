#include "ServerCorePch.h"
#include "Broadcaster.h"
#include "PacketSession.h"
#include "../SimplePacket.h"
#include "../ClientSession.h"
#include "../Horse.h"

//extern std::function<bool(ServerCore::Session*, ServerCore::Session*)> g_huristic;

namespace ServerCore
{
	Broadcaster::Broadcaster()
	{
	}

	Broadcaster::~Broadcaster()
	{
	}

	const int Broadcaster::BroadcastMove(
		const S_ptr<SendBuffer>& in_pkt,
		const S_ptr<SendBuffer>& out_pkt,
		const S_ptr<SendBuffer>& move_pkt,
		const S_ptr<IocpEntity>& thisSession_,
		const Vector<SessionManageable*>* const sectors
	)noexcept
	{
		if (IDLE != m_work_flag.exchange(WORK, std::memory_order_relaxed))
			return NONE;
		int sector_state = 0;
		
		const uint16 obj_type = thisSession_->GetObjectType();
		const auto thisSession = thisSession_->IsSession();
		if (thisSession)
		{
			if (false == thisSession->IsConnectedAtomic())
				return NONE;
		}
		const auto cache_obj_ptr = thisSession_.get();
		const bool bIsNPC = 0 != obj_type;

		thread_local HashSet<S_ptr<IocpEntity>> new_view_list;
		new_view_list.clear();
		for (const auto sector : *sectors)
		{
			sector->GetSRWLock().lock_shared();
			for (const auto pSession : sector->GetObjectList())
			{
				const bool bFlag = static_cast<const bool>(pSession->IsSession());
				if (bIsNPC && !bFlag)
					continue;
				if (auto pValid = pSession->GetSharedThis())
				{
					if (g_huristic(cache_obj_ptr, pSession))
					{
						new_view_list.emplace(std::move(pValid));
						sector_state |= (bFlag + 1);
					}
				}
			}
			sector->GetSRWLock().unlock_shared();
		}
		new_view_list.erase(thisSession_);

		for (const auto& pEntity : new_view_list)
		{
			if (!m_viewList.contains(pEntity))
			{
				// 추가하고 입장패킷을 보낸다
				//thisSession->SendAsync(in_pkt);
				if (const auto pSession = pEntity->IsSession())
					pSession->SendAsync(in_pkt);

				//pEntity->IsSession()->SendAsync(in_pkt);

				//SC_ADD_PLAYER_PACKET add_packet;
				{
					//const auto pClient = static_cast<ClientSession*>(pSession.get());
					//add_packet.id = (short)pSession->GetSessionID();
					//strcpy_s(add_packet.name, pClient->getName());
					//add_packet.pkt_size = sizeof(add_packet);
					//add_packet.pkt_id = SC_ADD_PLAYER;
					//add_packet.x = (short)pClient->GetHorse()->m_xPos;
					//add_packet.y = (short)pClient->GetHorse()->m_yPos;
				}
				if (thisSession)
					thisSession->SendAsync(g_create_in_pkt(pEntity));
				//thisSession->SendAsync(move_pkt);
				//pSession->SendAsync(move_pkt);
				//m_viewList.emplace(static_cast<PacketSession* const>(pSession));
				m_viewList.emplace(pEntity);
			}
			else
			{
				// 이동패킷을 보내면 된다
				if (thisSession)
					thisSession->SendAsync(move_pkt);
				if (const auto pSession = pEntity->IsSession())
					pSession->SendAsync(move_pkt);
				//pEntity->IsSession()->SendAsync(move_pkt);
			}
		}

		//m_viewList.sort([](const Session* const a, const Session* const b)noexcept {return *a < *b; });
		const auto e_iter = new_view_list.cend();
		//if (m_viewList.empty())return;
		for (auto iter = m_viewList.cbegin(); iter != m_viewList.cend();)
		{
			//const auto pEntity = iter->second.lock();
			const auto& pEntity = *iter;
			//if (nullptr == pEntity)
			//{
			//	iter = m_viewList.erase(iter);
			//	cont
			//}
			//const auto target = std::ranges::lower_bound(new_view_list, pSession->GetSessionID(), {}, &Session::GetSessionID);
			const auto target = new_view_list.find(pEntity);

			if (e_iter == target)
			{
				// 없앤다
				//thisSession->SendAsync(out_pkt);
				if (const auto pSession = pEntity->IsSession())
					pSession->SendAsync(out_pkt);
				//pEntity->IsSession()->SendAsync(out_pkt);
				//ServerCore::SC_REMOVE_PLAYER_PACKET remove_packet;
				//{
				//	
				//	//remove_packet.id = (short)pSession->GetSessionID();
				//	remove_packet.pkt_size = sizeof(remove_packet);
				//	remove_packet.pkt_id = SC_REMOVE_PLAYER;
				//}
				if (thisSession)
					thisSession->SendAsync(g_create_out_pkt(pEntity));
				iter = m_viewList.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		
		if (WORK != m_work_flag.exchange(IDLE, std::memory_order_release))
		{
			m_viewList.clear();
			return STOP;
		}
			
		return 3 ^ sector_state;
	}
}