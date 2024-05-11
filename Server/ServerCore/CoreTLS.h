#pragma once
#include <stack>

namespace ServerCore
{
	extern thread_local uint32 LThreadId;
	extern thread_local std::shared_ptr<class SendBufferChunk> LSendBufferChunk;
	extern thread_local uint64				LEndTickCount;
	extern thread_local class TaskQueueable* LCurTaskQueue;
	extern thread_local uint64				LCurHandleSessionID;
}