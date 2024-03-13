#include "ServerCorePch.h"
#include "PacketSession.h"

namespace ServerCore
{
	PacketSession::PacketSession(const PacketHandleFunc sessionPacketHandler_)noexcept
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
			const int32 dataSize = len - processLen; // �ι�°���ʹ� �̹� �س����� �ֱ� ����
			// �ּ��� ����� �Ľ��� �� �ִ°�?
			if (dataSize < static_cast<c_int32>(sizeof(PacketHeader)))
				break;

			PacketHeader* const header = reinterpret_cast<PacketHeader* const>(buffer + processLen);
			c_int32 packetSize = header->size;
			// ����� ��ϵ� ��Ŷũ�⸦ �Ľ��Ҽ��ֳ�?
			if (dataSize < packetSize)
				break;

			// �ϴ� ����ٿ�, ��������

			if (static_cast<c_int32>(HEART_BEAT::c2s_HEART_BEAT) == header->id)
			{
				SetHeartBeat(true);
			}
			else
			{
				bIsOk &= m_sessionPacketHandler(pThisSessionPtr, reinterpret_cast<BYTE* const>(header), packetSize);
			}

			processLen += packetSize;
		}

		return { processLen,bIsOk };
	}
}