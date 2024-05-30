#include "ServerCorePch.h"
#include "DBMgr.h"

namespace ServerCore
{
	DBMgr::DBMgr()
	{
	}

	DBMgr::~DBMgr()
	{
	}

	void DBMgr::Init() noexcept
	{
	}

	bool DBMgr::Connect(c_int32 connectionCount, const std::wstring_view connectionString)
	{
		SpinLockGuard lock{ m_spinLock };

		m_connections.reserve(connectionCount);

		if (::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_environment) != SQL_SUCCESS)
			return false;

		if (::SQLSetEnvAttr(m_environment, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0) != SQL_SUCCESS)
			return false;

		for (int32 i = 0; i < connectionCount; ++i)
		{
			DBConnectionHandle* const connection = xnew<DBConnectionHandle>();
			if (connection->Connect(m_environment, connectionString) == false)
				return false;

			m_connections.emplace_back(connection);
		}

		return true;
	}

	void DBMgr::Clear()
	{
		SpinLockGuard lock{ m_spinLock };

		if (m_environment != SQL_NULL_HANDLE)
		{
			::SQLFreeHandle(SQL_HANDLE_ENV, m_environment);
			m_environment = SQL_NULL_HANDLE;
		}

		for (const DBConnectionHandle* const connection : m_connections)
			xdelete(const_cast<DBConnectionHandle* const>(connection));

		m_connections.clear();
	}

	const DBConnectionHandle* const DBMgr::Pop()noexcept
	{
		SpinLockGuard lock{ m_spinLock };
		NAGOX_ASSERT(false == m_connections.empty());
		const DBConnectionHandle* const connection = m_connections.back();
		m_connections.pop_back();
		return connection;
	}

	void DBMgr::Push(const DBConnectionHandle* const connection)noexcept
	{
		SpinLockGuard lock{ m_spinLock };
		m_connections.emplace_back(connection);
	}
}