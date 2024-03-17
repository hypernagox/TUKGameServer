#include "pch.h"
#include "SimplePacket.h"
#include "TaskTimerMgr.h"
#include "ClientSession.h"
#include "Horse.h"

static ServerCore::S_ptr<ClientSession> GetClientSession(const ServerCore::S_ptr<ServerCore::PacketSession>& pSession_)
{
	return std::static_pointer_cast<ClientSession>(pSession_);
}

namespace ServerCore
{
	const bool c2s_LOGIN::Handle(const S_ptr<PacketSession>& pSession_, const c2s_LOGIN& pkt_)
	{
		s2c_LOGIN pkt;
		pkt.x = 0;
		pkt.y = 0;
		pkt.sessionID = pSession_->GetSessionID();
		pkt.vInitPos = Horse::GetInitPos();
		pSession_ << pkt;

		
		return true;
	}
	const bool s2c_LOGIN::Handle(const S_ptr<PacketSession>& pSession_, const s2c_LOGIN& pkt_)
	{
		return false;
	}

	const bool s2c_KEY::Handle(const S_ptr<PacketSession>& pSession_, const s2c_KEY& pkt_)
	{
		return false;
	}
	const bool c2s_KEY::Handle(const S_ptr<PacketSession>& pSession_, const c2s_KEY& pkt_)
	{
		const auto pSession = GetClientSession(pSession_);
		const auto newPos = pSession->GetHorse()->UpdateHorsePos(pkt_.VK);

		s2c_KEY pkt;
		pkt.x = newPos.first.x;
		pkt.y = newPos.first.y;
		pkt.vPos = newPos.second;
		pkt.moveUserID = pSession->GetSessionID();

		&Horse::g_board << pkt;
		pSession_ << pkt;
		return true;
	}
	const bool s2c_NEW_PLAYER::Handle(const S_ptr<PacketSession>& pSession_, const s2c_NEW_PLAYER& pkt_)
	{
		return false;
	}
	const bool c2s_NEW_PLAYER::Handle(const S_ptr<PacketSession>& pSession_, const c2s_NEW_PLAYER& pkt_)
	{
		Horse::g_board.EnqueueAsync([pSession_]()
			{
				const auto& pHorse = GetClientSession(pSession_)->GetHorse();
				s2c_NEW_PLAYER pkt;
				pkt.otherID = pSession_->GetSessionID();
				pkt.x = pHorse->m_xPos;
				pkt.y = pHorse->m_yPos;
				pkt.vOtherPos = pHorse->m_vPos;
				for (const auto others : Horse::g_board.GetSessionList())
				{
					{
						others << pkt;
					}
					{
						const auto other = static_cast<ClientSession*>(others);
						const auto& other_horse = other->GetHorse();
						s2c_NEW_PLAYER pkt;
						pkt.otherID = other->GetSessionID();
						pkt.x = other_horse->m_xPos;
						pkt.y = other_horse->m_yPos;
						pkt.vOtherPos = other_horse->m_vPos;
						pSession_ << pkt;
					}
				}
			});

		Horse::g_board.EnterEnqueue(pSession_);


		return true;
	}

	const bool c2s_LEAVE::Handle(const S_ptr<PacketSession>& pSession_, const c2s_LEAVE& pkt_)
	{

		s2c_LEAVE pkt;
		pkt.leaveUserID = pkt_.leaveUserID;

		&Horse::g_board << pkt;

		return true;
	}

	const bool s2c_LEAVE::Handle(const S_ptr<PacketSession>& pSession_, const s2c_LEAVE& pkt_)
	{
		return false;
	}
}
