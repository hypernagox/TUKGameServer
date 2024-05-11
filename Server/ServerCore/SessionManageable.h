#pragma once

namespace ServerCore
{
	class Session;
	class IocpEntity;
	class SendBuffer;
	using drop_range = std::ranges::drop_view<std::ranges::ref_view<std::list<ServerCore::Session*, ServerCore::AtomicAllocator<ServerCore::Session*>>>>;
	class SessionManageable
		:public TaskQueueable
	{
		enum { HEART_BEAT_TICK = 15000 };
	public:
		SessionManageable(const uint16_t roomID_)noexcept;
		virtual ~SessionManageable()noexcept;
		//void BroadCast(const S_ptr<SendBuffer> pSendBuffer)noexcept;
		constexpr inline auto& GetSRWLock()noexcept { return m_linkedHashMapForSession.GetSRWLock(); }
		void lock()noexcept { GetSRWLock().lock(); }
		void unlock()noexcept { GetSRWLock().unlock(); }
		void lock_shared()noexcept { GetSRWLock().lock_shared(); }
		void unlock_shared()noexcept { GetSRWLock().unlock_shared(); }
	public:
		const uint16_t GetRoomID()const noexcept { return m_roomID; }
		//const auto& GetSessionList()const noexcept { return m_linkedHashMapForSession[Mgr(ThreadMgr)->GetCurThreadIdx()].GetItemListRef(); }
		const auto& GetObjectList()const noexcept { return m_linkedHashMapForSession.GetItemListRef(); }

		void SendEnqueue(S_ptr<IocpEntity> pSession_, S_ptr<SendBuffer> pSendBuffer_)noexcept;
		void EnterEnqueue(S_ptr<IocpEntity> pSession_)noexcept;
		//void LeaveAndDisconnectEnqueue(S_ptr<IocpEntity> pSession_)noexcept;
		void LeaveAndDisconnectEnqueue(const uint64_t obj_id)noexcept;
		void BroadCastEnqueue(S_ptr<SendBuffer> pSendBuffer)noexcept;
		void BroadCastEnqueue(S_ptr<SendBuffer> pSendBuffer, c_uint64 exceptSessionNumber)noexcept;
		S_ptr<IocpEntity> FindSession(const uint64 sessionID) const noexcept;
		void ImmigrationEnqueue(S_ptr<SessionManageable> pOtherRoom, c_uint64 sessionID)noexcept;
		void ImmigrationAllEnqueue(S_ptr<SessionManageable> pOtherRoom, const bool bDestroyCurrentRoom)noexcept;

		const int MoveBroadCastEnqueue(
			const S_ptr<IocpEntity>& move_session,
			const S_ptr<SendBuffer>& in_pkt,
			const S_ptr<SendBuffer>& out_pkt,
			const S_ptr<SendBuffer>& move_pkt,
			const Vector<SessionManageable*>* sectors
		)noexcept;

	protected:
		//drop_range GetSessionRangeExceptOne(c_uint64 exceptSessionNumber_)noexcept;
		virtual void ImigrationAfterBehavior(const S_ptr<ServerCore::SessionManageable> beforeRoom, const S_ptr<ServerCore::IocpEntity> pSession_)noexcept abstract;
	public:
		void Enter(S_ptr<IocpEntity> pSession_)noexcept;
		void LeaveAndDisconnect(const uint64_t obj_id)noexcept;
		void Immigration(const S_ptr<SessionManageable> pOtherRoom, const uint64 sessionID) noexcept;
		void ImmigrationAll(const S_ptr<SessionManageable> pOtherRoom, const bool bDestroyCurrentRoom)noexcept;
		void BroadCast(const S_ptr<SendBuffer> pSendBuffer)noexcept;
		void BroadCastExceptOne(const S_ptr<SendBuffer> pSendBuffer, const uint64 exceptSessionNumber)noexcept;
		void RegisterHeartBeat()noexcept;
		void ListenHeartBeat()noexcept;

		const int MoveBroadCast(
			const S_ptr<IocpEntity>& move_session,
			const S_ptr<SendBuffer>& in_pkt,
			const S_ptr<SendBuffer>& out_pkt,
			const S_ptr<SendBuffer>& move_pkt,
			const Vector<SessionManageable*>* const sectors
			)noexcept;
	protected:
		LinkedHashMap<uint64, IocpEntity> m_linkedHashMapForSession{ 128 };
		LinkedHashMap<uint64, PacketSession> m_linkedHashMapForClient{ 128 };
		//HashMap<uint64_t,S_ptr<PacketSession>> m_linkedHashMapForClient;

		//std::vector<Session*> m_vecIterate[NUM_OF_THREADS];
		//LinkedHashMap<uint64, Session> m_linkedHashMapForSession;
		//SpinLock m_spinLock[NUM_OF_THREADS];
		Vector<Task*> m_vecTaskBulks;
		const uint16_t m_roomID;
		
	};
}
