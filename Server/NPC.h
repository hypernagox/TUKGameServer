#pragma once
#include "IocpObject.h"
#include "TimerObject.h"
#include "State.h"
#include "FSM.h"
#include "Lua/lua.hpp"

constexpr const bool g_bUSE_LUA = false; // false¸é ±×³É FSM

class NPC
	:public ServerCore::TimerObject
{
public:
	static inline ServerCore::HashMap<uint64_t, NPC*> g_mapNPC;
	static NPC* const GetNPC(const uint64_t npc_id)noexcept { return g_mapNPC[npc_id]; }
public:
	NPC();
	~NPC();
public:
	virtual void ToAwaker(const uint64_t awakerID_)noexcept override;
	virtual const ServerCore::TIMER_STATE TimerUpdate()noexcept override;
public:
	std::recursive_mutex m_srwLock;
	ServerCore::SRWLock m_srwLock2;
	ServerCore::HashSet<uint64_t> m_helloList;
	std::chrono::steady_clock::time_point m_time{};
	FSM m_fsm;
	lua_State* m_lua;
};

