#pragma once
#include "ServerCorePch.h"
#include "enable_shared_cache_this.hpp"
#include "Broadcaster.h"

class ContentsEntity;

namespace ServerCore
{
	class IocpEvent;
	class PacketSession;
	
	/*--------------
		IocpCore
	---------------*/

	// IOCP에 등록 가능한 모든 오브젝트
	class IocpObject
		:public enable_shared_cache_this_core<IocpObject>
	{
	public:
		IocpObject();
		virtual ~IocpObject();
	public:
		virtual HANDLE GetHandle()const noexcept { return nullptr; }
		virtual void Dispatch(IocpEvent* const iocpEvent_, c_int32 numOfBytes)noexcept {}
	};

	class IocpEntity
		:public IocpObject
	{
	public:
		const U_Pptr<Broadcaster> m_broadCaster=MakePoolUnique<Broadcaster>();
		IocpEntity(const uint16_t type_id,S_ptr<ContentsEntity> pEntity_) noexcept 
			: m_objectCombineID{ CombineObjectID(type_id,IDGenerator::GenerateID()) } 
			, m_pContentsEntity{ std::move(pEntity_) }
		{}
		virtual ~IocpEntity()noexcept = default;
	public:
		const S_ptr<ContentsEntity>& GetContentsEntity()const noexcept { return m_pContentsEntity; }
		const uint64_t GetObjectID()const noexcept { return ServerCore::GetObjectID(m_objectCombineID); }
		const uint16_t GetObjectType()const noexcept { return ServerCore::GetObjectType(m_objectCombineID); }
		PacketSession* const IsSession()noexcept
		{
			return reinterpret_cast<PacketSession* const>(
				static_cast<const int64_t>(-!GetObjectType()) &
				reinterpret_cast<const int64_t>(this)
				);
		}
		const PacketSession* const IsSession()const noexcept
		{
			return reinterpret_cast<const PacketSession* const>(
				static_cast<const int64_t>(-!GetObjectType()) &
				reinterpret_cast<const int64_t>(this)
				);
		}
		template<typename T>
		T* const ObjectCast(const uint16_t type_id)const noexcept
		{
			return reinterpret_cast<T* const>(
				static_cast<const int64_t>(-!!(GetObjectType() == type_id)) &
				reinterpret_cast<const int64_t>(this)
				);
		}
		inline S_ptr<IocpEntity> GetSharedThis()noexcept { return IocpObject::SharedCastThis<IocpEntity>(); }
		inline S_ptr<const IocpEntity> GetSharedThis()const noexcept { return IocpObject::SharedCastThis<IocpEntity>(); }
		//template<typename T>
		//const T* const ObjectCast(const uint16_t type_id)const noexcept 
		//{
		//	return reinterpret_cast<const T* const>(
		//		static_cast<const int64_t>(-!(GetObjectType() == type_id)) &
		//		reinterpret_cast<const int64_t>(this)
		//		);
		//}
	private:
		const S_ptr<ContentsEntity> m_pContentsEntity;
		const uint64_t m_objectCombineID;
	};
}