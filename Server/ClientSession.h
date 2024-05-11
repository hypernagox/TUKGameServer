#pragma once
#include "PacketSession.h"
#include "ContentsEntity.h"

class Horse;

class ClientSession
	:public ServerCore::PacketSession
{
public:
	ClientSession();
	ClientSession(const SOCKET sock);
	~ClientSession();
public:
	virtual void OnConnected() override;
	virtual void OnSend(c_int32 len)noexcept override;
	virtual void OnDisconnected()override;
public:
	//const std::shared_ptr<Horse>& GetHorse()const noexcept { return GetHorse(); }
	const char* const getName()const noexcept { return GetContentsEntity()->getName(); }
	char* const getName()noexcept { return GetContentsEntity()->getName(); }
public:
	unsigned int m_lastMoveTime = 0;
};

