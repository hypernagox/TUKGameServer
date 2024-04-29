#pragma once

namespace ServerCore
{
	class Session;
	class SendBuffer;

	class Broadcaster
	{
	public:
		Broadcaster();
		~Broadcaster();
	public:
		void BroadcastMove(
			const S_ptr<SendBuffer>& in_pkt,
			const S_ptr<SendBuffer>& out_pkt,
			const S_ptr<SendBuffer>& move_pkt,
			const S_ptr<Session>& thisSession,
			const Vector<SessionManageable*>& sectors
		)noexcept;
		void ReleaseViewList()noexcept
		{
			new_view_list.clear();
			m_viewList.clear();
		}
	public:
		SpinLock m_spinLock;
	private:
		//HashSet<PacketSession*> m_viewList2[NUM_OF_THREADS];
		HashSet<S_ptr<PacketSession>> new_view_list;
		HashSet<S_ptr<PacketSession>> m_viewList;
	};
}
