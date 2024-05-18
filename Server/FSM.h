#pragma once
#include "TimerObject.h"

class State;
class NPC;

class FSM
{
public:
	FSM()noexcept;
public:
	const ServerCore::TIMER_STATE FSMUpdate(NPC* const npc)noexcept;
	void ChangeState(NPC* const npc,const uint8 nextStateEnum_);
private:
	State* m_pCurState;
	ServerCore::HashMap<uint8_t, ServerCore::U_ptr<State>> m_mapState;
};

