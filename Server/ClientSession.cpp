#include "pch.h"
#include "ClientSession.h"
#include "c2s_PacketHandler.h"
#include "Horse.h"
#include "Chess.h"

ClientSession::ClientSession()
	: PacketSession{ ServerCore::c2s_PacketHandler::GetPacketHandlerList()}
	, m_pClientHorse{ ServerCore::MakeShared<Horse>() }
{
	m_pClientHorse->m_pSession = this;
}

ClientSession::~ClientSession()
{
	std::cout << "Destruct !!!\n";
}

void ClientSession::OnConnected()
{
}

void ClientSession::OnSend(c_int32 len) noexcept
{
}

void ClientSession::OnDisconnected()
{
	Horse::g_board.LeaveAndDisconnectEnqueue(SharedCastThis<Session>());
}
