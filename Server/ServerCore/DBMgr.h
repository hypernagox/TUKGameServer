#pragma once
#include <sql.h>
#include <sqlext.h>
#include "DBEvent.h"

namespace ServerCore
{
	class DBConnectionHandle;

	class DBMgr
		:public Singleton<DBMgr>
	{
		friend class Singleton;
		DBMgr();
		~DBMgr();
	public:
		void Init()noexcept override;
		bool Connect(c_int32 connectionCount, const std::wstring_view connectionString);
		void Clear();

		const DBConnectionHandle* const Pop()noexcept;
		void Push(const DBConnectionHandle* const connection)noexcept;

		void EnqueueDBEvent(S_ptr<DBEvent>&& dbEvent)noexcept
		{
			m_dbEventQueue.emplace(std::move(dbEvent));
		}
	private:
		void ExecuteQuery()noexcept;
	private:
		MPSCQueue<S_ptr<DBEvent>> m_dbEventQueue;
		SpinLock m_spinLock;
		SQLHENV	m_environment = SQL_NULL_HANDLE;
		Vector<const DBConnectionHandle*> m_connections;
		std::jthread m_queryThread;
	};
}

template <typename DBEVENT> requires std::derived_from<DBEVENT, ServerCore::DBEvent>
static void RequestQuery(DBEVENT& db)noexcept
{
	ServerCore::S_ptr<ServerCore::DBEvent> dbEvent = ServerCore::MakeShared<DBEVENT>(std::move(db));
	dbEvent->SetEventPtr(dbEvent);
	Mgr(DBMgr)->EnqueueDBEvent(std::move(dbEvent));
}

template <typename DBEVENT> requires std::derived_from<DBEVENT, ServerCore::DBEvent>
static void RequestQuery(DBEVENT&& db)noexcept
{
	ServerCore::S_ptr<ServerCore::DBEvent> dbEvent = ServerCore::MakeShared<DBEVENT>(std::move(db));
	dbEvent->SetEventPtr(dbEvent);
	Mgr(DBMgr)->EnqueueDBEvent(std::move(dbEvent));
}