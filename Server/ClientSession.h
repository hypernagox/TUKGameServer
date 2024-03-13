#pragma once
#include "PacketSession.h"

class Horse;

class ClientSession
	:public ServerCore::PacketSession
{
public:
	ClientSession();
	~ClientSession();
public:
	virtual void OnConnected() override;
	virtual void OnSend(c_int32 len)noexcept override;
	virtual void OnDisconnected()override;
public:
	const std::shared_ptr<Horse>& GetHorse()const noexcept { return m_pClientHorse; }
private:
	const std::shared_ptr<Horse> m_pClientHorse;
};

