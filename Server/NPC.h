#pragma once
#include "IocpObject.h"
#include "TimerObject.h"

class NPC
	:public ServerCore::TimerObject
{
public:
	NPC();
	~NPC();
public:
	virtual const ServerCore::TIMER_STATE TimerUpdate()noexcept override;
private:
	std::chrono::steady_clock::time_point m_time{};
};

