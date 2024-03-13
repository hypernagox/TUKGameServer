#pragma once
#include "PacketSession.h"

class MyHorse;

class ServerSession
	:public NetHelper::PacketSession
{
public:
	ServerSession();
	~ServerSession();
public:
	virtual void OnConnected() override;
	virtual void OnSend(c_int32 len)noexcept override;
	virtual void OnDisconnected()override;
public:
	void SetHorse(MyHorse* pHorse_) { m_pHorse = pHorse_; }
	MyHorse* GetHorse()const { return m_pHorse; }
private:
	MyHorse* m_pHorse;
};

