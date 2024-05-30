#pragma once
#include <sql.h>
#include <sqlext.h>

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

	private:
		SpinLock m_spinLock;
		SQLHENV	m_environment = SQL_NULL_HANDLE;
		Vector<const DBConnectionHandle*> m_connections;
	};
}
