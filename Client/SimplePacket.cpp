#include "pch.h"
#include "SimplePacket.h"
#include "NetworkMgr.h"
#include "MyHorse.h"
#include "ServerSession.h"

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
		GetServerSession()->GetHorse()->SetHorsePos(pkt_.y, pkt_.x, pkt_.vInitPos);

		return true;
	}
	const bool s2c_KEY::Handle(const S_ptr<PacketSession>& pSession_, const s2c_KEY& pkt_)
	{
		GetServerSession()->GetHorse()->SetHorsePos(pkt_.y, pkt_.x, pkt_.vPos);
		return true;
	}
	const bool c2s_KEY::Handle(const S_ptr<PacketSession>& pSession_, const c2s_KEY& pkt_)
	{
		return false;
	}
	
}
