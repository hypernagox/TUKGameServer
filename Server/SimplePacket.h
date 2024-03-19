#pragma once
#include "c2s_PacketHandler.h"
#include "PacketArray.hpp"

namespace ServerCore
{
    class PacketSession;
    class SendBuffer;

    enum class SIMPLE_PKT : uint16
    {
        c2s_LOGIN = 1000,
        s2c_LOGIN = 1001,

        c2s_NEW_PLAYER = 1002,
        s2c_NEW_PLAYER = 1003,

        c2s_KEY = 1004,
        s2c_KEY = 1005,


        c2s_LEAVE = 1006,
        s2c_LEAVE = 1007,
    };

#pragma pack (push, 1)

    template <typename T>
    struct SimplePacket
        :public PacketHeader
    {
    private:
        static bool InitSimplePacket()noexcept
        {
            T temp;
            c2s_PacketHandler::AddProtocol(static_cast<c_uint16>(temp.pkt_id), &T::HandlePacket);
            return true;
        }
        static const inline bool g_bInitPacket = InitSimplePacket();
    public:
        SimplePacket(const SIMPLE_PKT pktID_)noexcept
            :PacketHeader{ sizeof(T),static_cast<c_uint16>(pktID_) }
        {
        }

        bool SerializeToArray(void* const __restrict buffer, const size_t dataSize)const noexcept {
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

            S_ptr<SendBuffer> sendBuffer = Mgr(SendBufferMgr)->Open(packetSize);
            NAGOX_ASSERT(SerializeToArray(sendBuffer->Buffer(), packetSize));
            sendBuffer->Close(packetSize);

            return sendBuffer;
        }
    };

#pragma pack (pop)

    template <typename T, typename U>
    struct SendSimplePacketData
    {
        T first;
        U second;
    };

    template<typename T>
    static constexpr SendSimplePacketData<S_ptr<PacketSession>, S_ptr<SendBuffer>> operator + (S_ptr<PacketSession> pSendSession_, const SimplePacket<T>& pkt_)noexcept
    {
        return { std::move(pSendSession_), pkt_.MakeSendBuffer() };
    }

    template<typename T>
    static constexpr SendSimplePacketData<S_ptr<SendBuffer>, uint64> operator - (const SimplePacket<T>& pkt_, const S_ptr<PacketSession>& pSendSession_)noexcept
    {
        return { pkt_.MakeSendBuffer(),ServerCore::Session::GetID(pSendSession_) };
    }

    template<typename RoomPtr> requires std::convertible_to<RoomPtr, S_ptr<SessionManageable>> || std::convertible_to<RoomPtr, SessionManageable*>
    static constexpr void operator<<(RoomPtr&& pRoom_, SendSimplePacketData<S_ptr<PacketSession>, S_ptr<SendBuffer>>&& session_msg)noexcept
    {
        pRoom_->SendEnqueue(std::move(session_msg.first), std::move(session_msg.second));
    }

    template<typename RoomPtr> requires std::convertible_to<RoomPtr, S_ptr<SessionManageable>> || std::convertible_to<RoomPtr, SessionManageable*>
    static constexpr void operator<<(RoomPtr&& pRoom_, SendSimplePacketData<S_ptr<SendBuffer>, uint64>&& msg_exceptNum)noexcept
    {
        pRoom_->BroadCastEnqueue(std::move(msg_exceptNum.first), std::move(msg_exceptNum.second));
    }

    template<typename RoomPtr, typename T> requires std::convertible_to<RoomPtr, S_ptr<SessionManageable>> || std::convertible_to<RoomPtr, SessionManageable*>
    static constexpr void operator<<(RoomPtr&& pRoom_, const SimplePacket<T>& pkt_)noexcept
    {
        pRoom_->BroadCastEnqueue(pkt_.MakeSendBuffer());
    }

    template<typename SessionPtr, typename T> requires std::convertible_to<SessionPtr, S_ptr<Session>> || std::convertible_to<SessionPtr, Session*>
    static constexpr void operator<<(SessionPtr&& pSession_, const SimplePacket<T>& pkt_)noexcept
    {
        pSession_->SendAsync(pkt_.MakeSendBuffer());
    }

#pragma pack (push, 1)

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
        uint64_t sessionID;
        Vec2 vInitPos;
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const s2c_LOGIN& pkt_);
    };

    struct c2s_NEW_PLAYER
        :public SimplePacket<c2s_NEW_PLAYER>
    {
        c2s_NEW_PLAYER() :SimplePacket<c2s_NEW_PLAYER>{ SIMPLE_PKT::c2s_NEW_PLAYER } {}
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const c2s_NEW_PLAYER& pkt_);
    };

    struct s2c_NEW_PLAYER
        :public SimplePacket<s2c_NEW_PLAYER>
    {
        int x, y;
        uint64_t otherID;
        Vec2 vOtherPos;
        s2c_NEW_PLAYER() :SimplePacket<s2c_NEW_PLAYER>{ SIMPLE_PKT::s2c_NEW_PLAYER } {}
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const s2c_NEW_PLAYER& pkt_);
    };

    struct c2s_KEY
        :public SimplePacket<c2s_KEY>
    {
        c2s_KEY() :SimplePacket<c2s_KEY>{ SIMPLE_PKT::c2s_KEY } {}

        int VK;
        uint64_t moveUserID;
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const c2s_KEY& pkt_);
    };

    struct s2c_KEY
        :public SimplePacket<s2c_KEY>
    {
        s2c_KEY() :SimplePacket<s2c_KEY>{ SIMPLE_PKT::s2c_KEY } {}

        int x, y;
        Vec2 vPos;
        uint64_t moveUserID;
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const s2c_KEY& pkt_);
    };

    struct c2s_LEAVE
        :public SimplePacket<c2s_LEAVE>
    {
        c2s_LEAVE() :SimplePacket<c2s_LEAVE>{ SIMPLE_PKT::c2s_LEAVE } {}

        uint64_t leaveUserID;
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const c2s_LEAVE& pkt_);
    };

    struct s2c_LEAVE
        :public SimplePacket<s2c_LEAVE>
    {
        s2c_LEAVE() :SimplePacket<s2c_LEAVE>{ SIMPLE_PKT::s2c_LEAVE } {}

        uint64_t leaveUserID;
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const s2c_LEAVE& pkt_);
    };

#pragma pack (pop)
}
