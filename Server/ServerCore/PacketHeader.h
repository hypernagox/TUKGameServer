#pragma once
#include "ServerCorePch.h"

namespace ServerCore
{
#pragma pack (push, 1)
	struct PacketHeader
	{
		unsigned char pkt_size;
		char pkt_id;
	};
#pragma pack (pop)

	struct RecvStatus
	{
		c_int32 processLen;
		const bool bIsOK;
	};
}