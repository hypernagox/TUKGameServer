#pragma once
#include "ServerCorePch.h"

#pragma pack (push, 1)
struct DBPacketHeader
{
    uint8_t pkt_size;
    uint8_t pkt_id;
    wchar_t name[21]{};
};
#pragma pack (pop)

enum class DB_PKT :uint8
{
    PLAYER_MOVE,

    END
};

#pragma pack (push, 1)
template <typename T>
struct DBPacket
    :public DBPacketHeader
{
public:
    DBPacket(const DB_PKT pktID_)noexcept
        :DBPacketHeader{ sizeof(T),static_cast<c_uint8>(pktID_) }
    {
    }

    bool SerializeToArray(void* const __restrict buffer, const size_t dataSize)const noexcept {
        if (sizeof(T) != dataSize) [[unlikely]]
            return false;
        return ::memcpy(buffer, this, dataSize);
    }

    ServerCore::S_ptr<ServerCore::SendBuffer> MakeSendBuffer()const noexcept
    {
        const uint8_t packetSize = sizeof(T);

        ServerCore::S_ptr<ServerCore::SendBuffer> sendBuffer = Mgr(SendBufferMgr)->Open(packetSize);
        NAGOX_ASSERT(SerializeToArray(sendBuffer->Buffer(), packetSize));
        sendBuffer->Close(packetSize);

        return sendBuffer;
    }
};
#pragma pack (pop)

#pragma pack (push, 1)

struct s2q_PLAYER_MOVE
    :public DBPacket<s2q_PLAYER_MOVE>
{
    int x, y;
    s2q_PLAYER_MOVE() :DBPacket<s2q_PLAYER_MOVE>{ DB_PKT::PLAYER_MOVE } {}
};

#pragma pack (pop)