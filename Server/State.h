#pragma once
#include "pch.h"

enum class NPC_STATE : uint8
{
	IDLE,
	RANDOM_MOVE,
	HELLO_BYE,

	END,
};

class NPC;

class State
{
public:
	State(const NPC_STATE eState_):m_eState{eState_}{}
	virtual ~State();
public:
	virtual const NPC_STATE UpdateState(NPC* const npc)abstract;
	virtual void EnterState(NPC* const npc) {}
	virtual void ExitState(NPC* const npc) {}
public:
	const NPC_STATE GetState()const noexcept { return m_eState; }
private:
	const NPC_STATE m_eState;
};