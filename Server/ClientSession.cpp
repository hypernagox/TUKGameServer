#include "pch.h"
#include "ClientSession.h"
#include "c2s_PacketHandler.h"
#include "Horse.h"
#include "Chess.h"
#include "SimplePacket.h"
#include "ContentsEntity.h"

ClientSession::ClientSession()
	: PacketSession{ ServerCore::MakeShared<ContentsEntity>(),ServerCore::c2s_PacketHandler::GetPacketHandlerList()}
	//, ContentsEntity{ this }
{
}

ClientSession::ClientSession(const SOCKET sock)
	: PacketSession{ ServerCore::MakeShared<ContentsEntity>(),ServerCore::c2s_PacketHandler::GetPacketHandlerList(),sock }
	//, ContentsEntity{ this }
{
}

ClientSession::~ClientSession()
{
	// std::cout << "Destruct !!!\n";
}

void ClientSession::OnConnected()
{
}

void ClientSession::OnSend(c_int32 len) noexcept
{
}

void ClientSession::OnDisconnected()
{
	if (const auto pRoom = static_cast<Chess* const>(ExchangeSessionRoomInfo(0,0).GetPtr()))
	{
		m_broadCaster->ReleaseViewList();
		ServerCore::SC_REMOVE_PLAYER_PACKET p;
		p.id = (int)GetSessionID();
		p.pkt_size = sizeof(p);
		p.pkt_id = SC_REMOVE_PLAYER;

		if constexpr (400 == VIEW_RANGE)
		{
			pRoom->BroadCast(p.MakeSendBuffer());
		}
		else
		{
			for (const auto sectors : pRoom->m_adjVector)
			{
				//sectors->LeaveAndDisconnectEnqueue(GetObjectID());
				sectors << p;
			}
		}
		//m_broadCaster->ReleaseViewList();
		pRoom->LeaveAndDisconnectEnqueue(GetObjectID());
		reset_cache_shared();
		//m_broadCaster->ReleaseViewList();
	}
	else
	{
		m_broadCaster->ReleaseViewList();
		reset_cache_shared();
		//m_broadCaster->ReleaseViewList();
	}
	////Horse::g_board->BroadCast(p.MakeSendBuffer());
	//pRoom << p;
	//reset_cache_shared();
}
