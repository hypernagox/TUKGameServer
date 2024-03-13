#include "pch.h"
#include "ServerSession.h"
#include "NetworkMgr.h"
#include "SimplePacket.h"

ServerSession::ServerSession()
{
}

ServerSession::~ServerSession()
{
}

void ServerSession::OnConnected()
{
	NetHelper::c2s_LOGIN pkt;
	Send(pkt.MakeSendBuffer());
}

void ServerSession::OnSend(c_int32 len) noexcept
{
}

void ServerSession::OnDisconnected()
{
}
