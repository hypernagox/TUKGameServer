#pragma once
#include "ServerCorePch.h"
#include "AtomicMemoryPool.hpp"
#include "AtomicNonTemplate.h"
#include "Container.h"

class AtomicNonTemplate;

/*-------------
	Memory
---------------*/

namespace ServerCore
{
	class MemoryMgr
		:public Singleton<MemoryMgr>
	{
		friend class Singleton;
		enum
		{
			POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
			MAX_ALLOC_SIZE = 4096
		};
		MemoryMgr();
		~MemoryMgr();
	public:
		void* const	Allocate(const size_t size)const noexcept;
		void Release(void* const ptr)const noexcept;
	private:
		std::vector<AtomicNonTemplate*> m_pools;
		AtomicNonTemplate* m_poolTable[MAX_ALLOC_SIZE + 1];
		AtomicMemoryPool<AtomicNonTemplate> m_poolAllocator{ 64 };
	};

	template<typename Type, typename... Args>
	constexpr Type* const xnew(Args&&... args)noexcept
	{
		static_assert(alignof(Type) <= 16, "Type requires alignment greater than 16 bytes.");
		return std::construct_at<Type>(static_cast<Type* const>(Mgr(MemoryMgr)->Allocate(sizeof(Type))), std::forward<Args>(args)...);
	}

	template<typename Type>
	constexpr void xdelete(Type* const obj)noexcept
	{
		std::destroy_at<Type>(obj);
		Mgr(MemoryMgr)->Release(obj);
	}

	template<typename Type, typename... Args>
	constexpr S_ptr<Type> MakeShared(Args&&... args)noexcept
	{
		return std::allocate_shared<Type>(SharedAllocator<Type>{}, std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	constexpr U_ptr<T> MakeUnique(Args&&... args)noexcept
	{
		return std::unique_ptr<T, UDeleter<T>>{xnew<T>(std::forward<Args>(args)...), UDeleter<T>{}};
	}
}