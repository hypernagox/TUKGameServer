#pragma once

#define MULTI_IOCP
//#define SINGLE_IOCP
//#define OVERLAPPED_IO
//#define NON_BLOCK
//#define SOCK_TH

namespace ServerCore
{
#ifdef MULTI_IOCP
	constexpr const size_t NUM_OF_THREADS = 20 - 1 + 1;
#endif

#ifdef SINGLE_IOCP
	constexpr const size_t NUM_OF_THREADS = 1;
#endif

#ifdef OVERLAPPED_IO
	constexpr const size_t NUM_OF_THREADS = 1;
#endif
#if defined(NON_BLOCK) || defined(SOCK_TH)
	constexpr const size_t NUM_OF_THREADS = 6 - 1 + 1;
#endif
}

#include "ServerCorePch.h"
#include "ServerCore/func.h"
#include "SimpleMath.hpp"
#include "Vec2.hpp"
#include "Vec2Int.hpp"
#include "func.h"
