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

		pSession << pkt;

		return true;
	}
}
