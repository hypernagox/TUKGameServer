#pragma once
//#include "../Protocols/c2s_PacketHandler.h"
#include "inc/Protocols/c2s_PacketHandler.h"
#include "PacketArray.hpp"

constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;

constexpr int MAX_USER = 5000;

constexpr int W_WIDTH = 400*5;
constexpr int W_HEIGHT = 400*5;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_PLAYER = 3;
constexpr char SC_REMOVE_PLAYER = 4;
constexpr char SC_MOVE_PLAYER = 5;

namespace ServerCore
{
    class PacketSession;
    class SendBuffer;

    enum class SIMPLE_PKT : int8
    {
        CS_LOGIN = 0,
        CS_MOVE = 1,
        SC_LOGIN_INFO = 2,
        SC_ADD_PLAYER = 3,
        SC_REMOVE_PLAYER = 4,
        SC_MOVE_PLAYER = 5,

        END
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
            :PacketHeader{ sizeof(T),static_cast<c_int8>(pktID_) }
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
    struct CS_LOGIN_PACKET 
        :public SimplePacket<CS_LOGIN_PACKET>
    {
        char	name[NAME_SIZE];
        CS_LOGIN_PACKET() :SimplePacket<CS_LOGIN_PACKET>{ SIMPLE_PKT::CS_LOGIN }{}
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const CS_LOGIN_PACKET& pkt_);
    };

    struct CS_MOVE_PACKET
        :public SimplePacket<CS_MOVE_PACKET>
    {
        char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
        unsigned	move_time;
        CS_MOVE_PACKET():SimplePacket<CS_MOVE_PACKET>{SIMPLE_PKT::CS_MOVE}{}
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const CS_MOVE_PACKET& pkt_);
    };

    struct SC_LOGIN_INFO_PACKET
        :public SimplePacket<SC_LOGIN_INFO_PACKET>
    {
        int	id;
        short	x, y;
        SC_LOGIN_INFO_PACKET():SimplePacket<SC_LOGIN_INFO_PACKET>{SIMPLE_PKT::SC_LOGIN_INFO}{}
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const SC_LOGIN_INFO_PACKET& pkt_);
    };

    struct SC_ADD_PLAYER_PACKET
        :public SimplePacket< SC_ADD_PLAYER_PACKET>
    {
        int	id;
        short	x, y;
        char	name[NAME_SIZE];
        SC_ADD_PLAYER_PACKET():SimplePacket<SC_ADD_PLAYER_PACKET>{SIMPLE_PKT::SC_ADD_PLAYER}{}
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const SC_ADD_PLAYER_PACKET& pkt_);
    };

    struct SC_REMOVE_PLAYER_PACKET 
        :public SimplePacket< SC_REMOVE_PLAYER_PACKET >
    {
        int	id;
        SC_REMOVE_PLAYER_PACKET():SimplePacket<SC_REMOVE_PLAYER_PACKET>{SIMPLE_PKT::SC_REMOVE_PLAYER}{}
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const SC_REMOVE_PLAYER_PACKET& pkt_);
    };

    struct SC_MOVE_PLAYER_PACKET 
        :public SimplePacket< SC_MOVE_PLAYER_PACKET  >
    {
        int	id;
        short	x, y;
        unsigned int move_time;
        SC_MOVE_PLAYER_PACKET():SimplePacket<SC_MOVE_PLAYER_PACKET>{SIMPLE_PKT::SC_MOVE_PLAYER}{}
        static const bool Handle(const S_ptr<PacketSession>& pSession_, const SC_MOVE_PLAYER_PACKET& pkt_);
    };

#pragma pack (pop)
}
