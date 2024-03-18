#include "ServerCorePch.h"
#include "PacketSession.h"

namespace ServerCore
{
	PacketSession::PacketSession(const PacketHandleFunc* const sessionPacketHandler_)noexcept
		:Session{ sessionPacketHandler_ }
	{
	}

	PacketSession::~PacketSession()
	{
	}

	const RecvStatus PacketSession::OnRecv(BYTE* const buffer, c_int32 len, const S_ptr<PacketSession>& pThisSessionPtr)noexcept
	{
		int32 processLen = 0;
		bool bIsOk = true;
		for (;;)
		{
			const int32 dataSize = len - processLen; 
			
			if (dataSize < static_cast<c_int32>(sizeof(PacketHeader)))
				break;

			const PacketHeader* const __restrict header = reinterpret_cast<const PacketHeader* const>(buffer + processLen);
			c_int32 packetSize = header->size;
			c_int32 packetId = header->id;
			
			if (dataSize < packetSize)
				break;

			
			if (static_cast<c_int32>(HEART_BEAT::c2s_HEART_BEAT) == packetId)
			{
				SetHeartBeat(true);
			}
			else
			{
				bIsOk &= m_sessionPacketHandler[packetId](pThisSessionPtr, reinterpret_cast<BYTE* const>(const_cast<PacketHeader* const>(header)), packetSize);
				//bIsOk &= m_sessionPacketHandler(pThisSessionPtr, reinterpret_cast<BYTE* const>(const_cast<PacketHeader* const>(header)), packetSize);
			}

			processLen += packetSize;
		}

		return { processLen,bIsOk };
	}
}