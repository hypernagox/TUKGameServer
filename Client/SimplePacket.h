#pragma once
#include "s2c_PacketHandler.h"
#include "PacketArray.hpp"

namespace NetHelper
{
    class PacketSession;
    class SendBuffer;

    enum class SIMPLE_PKT : uint16
    {
        c2s_LOGIN = 1000,
        s2c_LOGIN = 1001,

        c2s_KEY = 1002,
        s2c_KEY = 1003,

    };

    template <typename T>
    struct SimplePacket
        :public PacketHeader
    {
    private:
        static bool InitSimplePacket()noexcept
        {
            T temp;
            s2c_PacketHandler::AddProtocol(static_cast<c_uint16>(temp.id), &T::HandlePacket);
            return true;
        }
        static const inline bool g_bInitPacket = InitSimplePacket();
    public:
        SimplePacket(const SIMPLE_PKT pktID_)noexcept
            :PacketHeader{ sizeof(T),static_cast<c_uint16>(pktID_) }
        {
        }

        bool SerializeToArray(void* const buffer, const size_t dataSize)const noexcept {
            if (sizeof(T) != dataSize) [[unlikely]]
                return false;
            return ::memcpy(buffer, this, dataSize);
        }

        static const bool HandlePacket(const S_ptr<PacketSession>& pSession_, BYTE* const pBuff_, c_int32 len_) {
            if (sizeof(T) != len_) [[unlikely]]
                return false;
            return T::Handle(pSession_, *reinterpret_cast<const T* const>(pBuff_));
        }

        S_ptr<SendBuffer> MakeSendBuffer()const noexcept
        {
            const uint16 packetSize = sizeof(T);

            S_ptr<SendBuffer> sendBuffer = NetMgr(SendBufferMgr)->Open(packetSize);
            NET_NAGOX_ASSERT(SerializeToArray(sendBuffer->Buffer(), packetSize));
            sendBuffer->Close(packetSize);

            return sendBuffer;
        }
    };

    template <typename T, typename U>
    struct SendSimplePacketData
    {
        T first;
        U second;
    };

    struct c2s_LOGIN
        :public SimplePacket<c2s_LOGIN>
    {
        c2s_LOGIN() :SimplePacket<c2s_LOGIN>{ SIMPLE_PKT::c2s_LOGIN } {}


        static const bool Handle(const S_ptr<PacketSession>& pSession_, const c2s_LOGIN& pkt_);
    };

    struct s2c_LOGIN
        :public SimplePacket<s2c_LOGIN>
    {
        s2c_LOGIN() :SimplePacket<s2c_LOGIN>{ SIMPLE_PKT::s2c_LOGIN } {}

        int x, y;
        Vec2 vInitPos;
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const s2c_LOGIN& pkt_);
    };

    struct c2s_KEY
        :public SimplePacket<c2s_KEY>
    {
        c2s_KEY() :SimplePacket<c2s_KEY>{ SIMPLE_PKT::c2s_KEY } {}

        int VK;
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const c2s_KEY& pkt_);
    };

    struct s2c_KEY
        :public SimplePacket<s2c_KEY>
    {
        s2c_KEY() :SimplePacket<s2c_KEY>{ SIMPLE_PKT::s2c_KEY } {}

        int x, y;
        Vec2 vPos;
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const s2c_KEY& pkt_);
    };

 
}
