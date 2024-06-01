#pragma once
#include "ServerCorePch.h"
#include "ThreadMgr.h"

namespace ServerCore
{
	struct EBRNode
	{
		uint64_t remove_point;
	};

	template <typename T>
	struct alignas(64) CacheLineSeperator
	{
		T data;
	};

	template <typename T> requires std::derived_from<T,EBRNode>
	class EBR
	{
	public:
		void Start()noexcept
		{
			auto& thCounter = m_thCounter[Mgr(ThreadMgr)->GetCurThreadIdx()].data;
			thCounter.store(
				m_eCounter.fetch_add(1, std::memory_order_relaxed),
				std::memory_order_relaxed
			);
		}
		void End()noexcept
		{
			auto& thCounter = m_thCounter[Mgr(ThreadMgr)->GetCurThreadIdx()].data;
			thCounter.store(0, std::memory_order_relaxed);
		}
		template <typename... Args>
		T* const GetNewNode(Args&&... args)noexcept
		{
			auto& freeList = m_freeList[Mgr(ThreadMgr)->GetCurThreadIdx()].data;
			if (freeList.empty())return xnew<T>(std::forward<Args>(args)...);
			T* const node = freeList.front();
			if(node->remove_point >= GetMinEpoch())return xnew<T>(std::forward<Args>(args)...);
			freeList.pop();
			return std::construct_at<T>(node, std::forward<Args>(args)...);
		}
		void RemoveNode(T* const node)noexcept
		{
			auto& freeList = m_freeList[Mgr(ThreadMgr)->GetCurThreadIdx()].data;
			std::destroy_at<T>(node);
			node->remove_point = GetMaxEpoch();
			freeList.emplace(node);
		}
	private:
		const uint64_t GetMaxEpoch()const noexcept {
			uint64_t max_epoch = 0;
			for (int i = 0; i < NUM_OF_THREADS; ++i)
				max_epoch = std::max(max_epoch, m_thCounter[i].data.load(std::memory_order_relaxed));
			return max_epoch;
		}
		const uint64_t GetMinEpoch()const noexcept {
			uint64_t min_epoch = std::numeric_limits<uint64_t>::max();
			for (int i = 0; i < NUM_OF_THREADS; ++i)
			{
				const uint64_t e = m_thCounter[i].data.load(std::memory_order_relaxed);
				if (0 != e)min_epoch = std::min(min_epoch, e);
			}
			return min_epoch;
		}
	private:
		std::atomic<uint64_t> m_eCounter = 0;
		CacheLineSeperator<std::atomic<uint64_t>> m_thCounter[NUM_OF_THREADS];
		CacheLineSeperator<Queue<T*>> m_freeList[NUM_OF_THREADS];
	};
}