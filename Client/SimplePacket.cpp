#include "pch.h"
#include "SimplePacket.h"
#include "NetworkMgr.h"
#include "MyHorse.h"
#include "ServerSession.h"
#include "Chess.h"

static std::shared_ptr<ServerSession> GetServerSession()
{
	return std::static_pointer_cast<ServerSession>(NetMgr(NetworkMgr)->GetSession());
}

namespace NetHelper
{
	const bool c2s_LOGIN::Handle(const S_ptr<PacketSession>& pSession_, const c2s_LOGIN& pkt_)
	{
		return false;
	}

	const bool s2c_LOGIN::Handle(const S_ptr<PacketSession>& pSession_, const s2c_LOGIN& pkt_)
	{
		
		NetMgr(NetworkMgr)->SetConnect();
		NetMgr(NetworkMgr)->SetSessionID(pkt_.sessionID);
		GetServerSession()->GetHorse()->SetHorsePos(pkt_.y, pkt_.x, pkt_.vInitPos);

		c2s_NEW_PLAYER pkt;

		Send(pkt);
		std::cout << "Connect !\n";
		return true;
	}
	const bool s2c_KEY::Handle(const S_ptr<PacketSession>& pSession_, const s2c_KEY& pkt_)
	{
		const auto pSession = GetServerSession();
		const auto& others = pSession->GetHorse()->GetBoard()->GetOtherPlayerMap();
		const auto iter = others.find(pkt_.moveUserID);
		Horse* target;
		if (others.end() != iter)
		{
			target = iter->second;
		}
		else
		{
			target = pSession->GetHorse();
		}
		target->SetHorsePos(pkt_.y, pkt_.x, pkt_.vPos);
		return true;
	}
	const bool c2s_KEY::Handle(const S_ptr<PacketSession>& pSession_, const c2s_KEY& pkt_)
	{
		return false;
	}
	
	const bool c2s_NEW_PLAYER::Handle(const S_ptr<PacketSession>& pSession_, const c2s_NEW_PLAYER& pkt_)
	{
		return false;
	}

	const bool s2c_NEW_PLAYER::Handle(const S_ptr<PacketSession>& pSession_, const s2c_NEW_PLAYER& pkt_)
	{
		GetServerSession()->GetHorse()->GetBoard()->AddNewPlayer(pkt_.otherID, pkt_.y, pkt_.x, pkt_.vOtherPos);

		return true;
	}

	const bool c2s_LEAVE::Handle(const S_ptr<PacketSession>& pSession_, const c2s_LEAVE& pkt_)
	{
		return false;
	}

	const bool s2c_LEAVE::Handle(const S_ptr<PacketSession>& pSession_, const s2c_LEAVE& pkt_)
	{
		GetServerSession()->GetHorse()->GetBoard()->LeavePlayer(pkt_.leaveUserID);
		return true;
	}

}
