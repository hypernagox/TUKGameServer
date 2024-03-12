#include "ClientNetworkPch.h"
#include "PacketSession.h"

namespace NetHelper
{
	PacketSession::PacketSession()
	{
	}

	PacketSession::~PacketSession()
	{
	}

	c_int32 PacketSession::OnRecv(BYTE* const buffer, c_int32 len)
	{
		int32 processLen = 0;
		for (;;)
		{
			const int32 dataSize = len - processLen;

			if (dataSize < static_cast<c_int32>(sizeof(PacketHeader)))
				break;

			PacketHeader* const header = reinterpret_cast<PacketHeader* const>(buffer + processLen);
			c_int32 packetSize = header->size;

			if (dataSize < packetSize)
				break;


			if (static_cast<c_int32>(HEART_BEAT::s2c_HEART_BEAT) == header->id)
			{
				Send(CreateHeartBeatSendBuffer(HEART_BEAT::c2s_HEART_BEAT));
			}
			else
			{
				m_sessionPacketHandler(GetCacheSharedFromThis(), reinterpret_cast<BYTE* const>(header), packetSize);
			}

			processLen += packetSize;
		}

		return processLen;
	}
}