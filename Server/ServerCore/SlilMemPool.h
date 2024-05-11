#pragma once

namespace ServerCore
{
	template <typename T>
	struct alignas(64) CacheLineWrapper
	{
		T data;
	};

	class SlilMemPool
	{
		using MemStack = CacheLineWrapper<ServerCore::Vector<void*>>;
	public:
		explicit SlilMemPool(const int mem_size_)noexcept :
			mem_size{ static_cast<const size_t>(mem_size_ + 7) & ~7} {}
	public:
		void* const Pop()noexcept {
			if (true == mem_stack.empty())
				return ::_aligned_malloc(mem_size,8);
			const auto node = mem_stack.back();
			mem_stack.pop_back();
			return node;
		}
		void Push(void* const ptr)noexcept {
			mem_stack.emplace_back(ptr);
		}
	public:
		const size_t mem_size;
		std::vector<void*> mem_stack;
	};
}

