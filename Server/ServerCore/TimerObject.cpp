#include "ServerCorePch.h"
#include "TimerObject.h"
#include "TaskTimerMgr.h"

namespace ServerCore
{
	void TimerObject::InitTimer(const S_ptr<TimerObject>& forCacheThis_, const uint64 tick_ms) noexcept
	{
		// 초기화시 필요한 추가동작은 여기에

		register_cache_shared_core(forCacheThis_);
		m_timerEvent.SetIocpObject(forCacheThis_);
		m_tickInterval = tick_ms;

	}

	const bool TimerObject::ExecuteTimer()noexcept
	{
		const TIMER_STATE ePrevState = m_timer_state.exchange(TIMER_STATE::RUN, std::memory_order_relaxed);
		if (TIMER_STATE::IDLE == ePrevState)
		{
			if (true == m_bStopFlag.load(std::memory_order_relaxed))
				return false;
			::PostQueuedCompletionStatus(Mgr(ThreadMgr)->GetIocpHandle(), 0, 0, &m_timerEvent);
			return true;
			//Mgr(TaskTimerMgr)->ReserveAsyncTask(m_tickInterval, &m_timerEvent);
		}
		else if (TIMER_STATE::PREPARE == ePrevState)
		{
			Mgr(TaskTimerMgr)->ReserveAsyncTask(m_tickInterval, &m_timerEvent);
			return true;
		}
		return false;
	}

	void TimerObject::Dispatch(IocpEvent* const iocpEvent_, c_int32 numOfBytes) noexcept
	{
		if (true == m_bStopFlag.load(std::memory_order_relaxed))
		{
			std::atomic_thread_fence(std::memory_order_acquire);
			reset_cache_shared();
			m_timerEvent.ReleaseIocpObject();
			return;
		}
		//const TIMER_STATE ePrevState = m_timer_state.exchange(TimerUpdate(), std::memory_order_relaxed);
		//const TIMER_STATE eCurState = m_timer_state.load(std::memory_order_relaxed);
		const TIMER_STATE eCurState = TimerUpdate();
		std::atomic_thread_fence(std::memory_order_seq_cst);

		m_timer_state.store(TIMER_STATE::PREPARE, std::memory_order_relaxed);
		const TIMER_STATE ePrevState = m_timer_state.exchange(eCurState, std::memory_order_relaxed);
		if (TIMER_STATE::RUN == eCurState && TIMER_STATE::PREPARE == ePrevState)
		{
			Mgr(TaskTimerMgr)->ReserveAsyncTask(m_tickInterval, &m_timerEvent);
		}
	}
}
