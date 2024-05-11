#pragma once

namespace ServerCore
{
	class IocpEntity;
	class Session;
	class SendBuffer;
	class PacketSession;

	struct BroadCastInfo
	{
		S_ptr<SendBuffer> in_pkt;
		S_ptr<SendBuffer> out_pkt;
		S_ptr<SendBuffer> move_pkt;
		S_ptr<IocpEntity> thisSession;
		const Vector<SessionManageable*>* sectors;
	};

	enum SECTOR_STATE
	{
		NOT_EMPTY = 0,
		NPC_EMPTY = 1 << 0,
		USER_EMPTY = 1 << 1,
		EMPTY = 3,
		STOP = 1 << 2,

		IDLE,
		WORK,

		NONE,
	};

	class Broadcaster
	{
	public:
		Broadcaster();
		~Broadcaster();
		using HuristicFunc = bool(*)(const IocpEntity* const, const IocpEntity* const);
		using PacketFunc = S_ptr<SendBuffer>(*)(const S_ptr<IocpEntity>&);
	public:

		const int BroadcastMove(
			const S_ptr<SendBuffer>& in_pkt,
			const S_ptr<SendBuffer>& out_pkt,
			const S_ptr<SendBuffer>& move_pkt,
			const S_ptr<IocpEntity>& thisSession,
			const Vector<SessionManageable*>* const sectors
		)noexcept;
		
		void ReleaseViewList()noexcept
		{
			if (IDLE == m_work_flag.exchange(STOP, std::memory_order_relaxed))
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				m_viewList.clear();
			}
		}
	public:
		//static void RegisterHuristicFunc(std::function<const bool(const IocpEntity* const, const IocpEntity* const)> fp_)noexcept {
		//	if (g_huristic)return;
		//	g_huristic.swap(fp_);
		//}
		//static void RegisterInPacketFunc(std::function<S_ptr<SendBuffer>(const S_ptr<IocpEntity>&)> fp_)noexcept {
		//	if (g_create_in_pkt)return;
		//	g_create_in_pkt.swap(fp_);
		//}
		//static void RegisterOutPacketFunc(std::function<S_ptr<SendBuffer>(const S_ptr<IocpEntity>&)> fp_)noexcept {
		//	if (g_create_out_pkt)return;
		//	g_create_out_pkt.swap(fp_);
		//}
		static void RegisterHuristicFunc(const HuristicFunc fp_)noexcept {
			if (g_huristic)return;
			g_huristic = (fp_);
		}
		static void RegisterInPacketFunc(const PacketFunc fp_)noexcept {
			if (g_create_in_pkt)return;
			g_create_in_pkt = (fp_);
		}
		static void RegisterOutPacketFunc(const PacketFunc fp_)noexcept {
			if (g_create_out_pkt)return;
			g_create_out_pkt = (fp_);
		}
		//bool m_bUpdateFlag = false;
	private:
		std::atomic<SECTOR_STATE> m_work_flag = IDLE; //0 stop 1 idle 3 run
		HashSet<S_ptr<IocpEntity>> m_viewList;
	private:
		static inline HuristicFunc g_huristic = {};
		static inline PacketFunc g_create_in_pkt = {};
		static inline PacketFunc g_create_out_pkt = {};
		//static inline std::function<const bool(const IocpEntity*const, const IocpEntity*const)> g_huristic = {};
		//static inline std::function<S_ptr<SendBuffer>(const S_ptr<IocpEntity>&)> g_create_in_pkt = {};
		//static inline std::function<S_ptr<SendBuffer>(const S_ptr<IocpEntity>&)> g_create_out_pkt = {};
	};
}
