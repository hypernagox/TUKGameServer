#include "ServerCorePch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"
#include "IocpEvent.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"
#include "PacketSession.h"


namespace ServerCore
{
#ifdef MULTI_IOCP
	const auto recv_callback = nullptr;
	const auto send_callback = nullptr;
#endif

#ifdef SINGLE_IOCP
	const auto recv_callback = nullptr;
	const auto send_callback = nullptr;
#endif

#ifdef OVERLAPPED_IO
	const auto recv_callback = RecvCompletionRoutine;
	const auto send_callback = SendCompletionRoutine;
#endif
#if defined(NON_BLOCK) || defined(SOCK_TH)
	const auto recv_callback = nullptr;
	const auto send_callback = nullptr;
#endif
	

	Session::Session(S_ptr<ContentsEntity> pEntity_,const PacketHandleFunc* const sessionPacketHandler_)noexcept
		: IocpEntity{ 0 ,std::move(pEntity_) }
		, m_pRecvEvent{ MakePoolUnique<RecvEvent>() }
		, m_pConnectEvent{ MakeUnique<ConnectEvent>() }
		, m_pDisconnectEvent{ MakePoolUnique<DisconnectEvent>() }
		, m_pSendEvent{ MakePoolUnique<SendEvent>() }
		, m_pRecvBuffer{ MakePoolUnique<RecvBuffer>(RecvBuffer::BUFFER_SIZE) }
		, m_sessionSocket{ SocketUtils::CreateSocket() }
		, m_sessionPacketHandler{ sessionPacketHandler_ }
		, m_sessionSocketForRecv{ m_sessionSocket }
	{
	}

	Session::Session(S_ptr<ContentsEntity> pEntity_,const PacketHandleFunc* const sessionPacketHandler_, const SOCKET sock) noexcept
		: IocpEntity{ 0 ,std::move(pEntity_) }
		, m_pRecvEvent{ MakePoolUnique<RecvEvent>() }
		, m_pConnectEvent{ MakeUnique<ConnectEvent>() }
		, m_pDisconnectEvent{ MakePoolUnique<DisconnectEvent>() }
		, m_pSendEvent{ MakePoolUnique<SendEvent>() }
		, m_pRecvBuffer{ MakePoolUnique<RecvBuffer>(RecvBuffer::BUFFER_SIZE) }
		, m_sessionSocket{ sock }
		, m_sessionPacketHandler{ sessionPacketHandler_ }
		, m_sessionSocketForRecv{ sock }
	{
	}

	Session::~Session()
	{
		//CancelIoEx(reinterpret_cast<HANDLE>(m_sessionSocket), NULL);
		//shutdown(m_sessionSocket, SD_BOTH);
		SocketUtils::Close(m_sessionSocket);
		//m_sessionSocketForRecv = INVALID_SOCKET;
	}

	void Session::SendAsync(S_ptr<SendBuffer> pSendBuff_) noexcept
	{
#if defined(NON_BLOCK) || defined(SOCK_TH)
		::send(m_sessionSocket, (char*)pSendBuff_->Buffer(), pSendBuff_->WriteSize(), 0);
		return;
#endif
		{
			m_sendQueue.emplace(std::move(pSendBuff_));
			if (false == m_bIsSendRegistered.exchange(true, std::memory_order_relaxed))
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				//if (false == m_bConnectedNonAtomic)
				//	return;
				//auto pThisSessionPtr = GetSharedThis();
				//if (nullptr == pThisSessionPtr)
				//	return;
				//::PostQueuedCompletionStatus(Mgr(ThreadMgr)->GetIocpHandle(), 0, 0, reinterpret_cast<IocpEvent* const>(reinterpret_cast<char* const>(m_pSendEvent.get()) + sizeof(IocpEvent)));
				
				#ifdef MULTI_IOCP
				auto pValid = shared_from_this();
				if (nullptr == pValid)
					return;
				m_pSendEvent->m_registerSendEvent.SetIocpObject(std::move(pValid));
				::PostQueuedCompletionStatus(Mgr(ThreadMgr)->GetIocpHandle(), 0, 0, reinterpret_cast<IocpEvent* const>(reinterpret_cast<char* const>(m_pSendEvent.get()) + sizeof(IocpEvent)));
				#endif
				
				#ifdef SINGLE_IOCP
				RegisterSend(std::static_pointer_cast<PacketSession>(shared_from_this()));
				#endif
				
				#ifdef OVERLAPPED_IO
				RegisterSend(std::static_pointer_cast<PacketSession>(shared_from_this()));
				#endif
			}
		}
	}

	bool Session::Connect()
	{
		return RegisterConnect();
	}

	bool Session::Disconnect(std::wstring_view cause)noexcept
	{
		LOG_MSG(std::move(cause));
		if (false == m_bConnected.exchange(false, std::memory_order_relaxed))
			return false;
		m_bConnectedNonAtomicForRecv = m_bConnectedNonAtomic = false;
		RegisterDisconnect();
		return true;
	}

	bool Session::SetNagle(const bool bTrueIsOff_FalseIsOn)const noexcept
	{
		return SocketUtils::SetTcpNoDelay(m_sessionSocket, bTrueIsOff_FalseIsOn);
	}

	void Session::Dispatch(IocpEvent* const iocpEvent_, c_int32 numOfBytes)noexcept
	{
		if (S_ptr<PacketSession> pThisSessionPtr{ iocpEvent_->PassIocpObject(),static_cast<PacketSession* const>(this) })
		{
			(this->*g_sessionLookupTable[static_cast<const uint8_t>(iocpEvent_->GetEventType())])(pThisSessionPtr, numOfBytes);
		}
	}

	bool Session::RegisterConnect()
	{
		if (IsConnected())
			return false;
		if (SERVICE_TYPE::CLIENT != GetService()->GetServiceType())
			return false;
		if (false == SocketUtils::SetReuseAddress(m_sessionSocket, true))
			return false;
		if (false == SocketUtils::BindAnyAddress(m_sessionSocket, 0))
			return false;

		//register_cache_shared(); 어디선가 누가 레지스터 해주긴 해야함
		m_pConnectEvent->Init();
		m_pConnectEvent->SetIocpObject(shared_from_this());

		const SOCKADDR_IN& sockAddr = GetService()->GetNetAddress().GetSockAddr(); // 내가 붙어야 할 서버쪽 주소임

		//DWORD numOfBytes = 0;

		if (false == SocketUtils::ConnectEx(m_sessionSocket, reinterpret_cast<const SOCKADDR* const>(&sockAddr), sizeof(sockAddr), NULL, NULL, NULL, m_pConnectEvent.get()))
		{
			const int32 errorCode = ::WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				m_pConnectEvent->ReleaseIocpObject();
				return false;
			}
		}

		return true;
	}

	void Session::ProcessConnect(const S_ptr<PacketSession>& pThisSessionPtr, c_int32 numofBytes_)noexcept
	{
		// 세선 등록
		if (GetService()->AddSession(pThisSessionPtr))
		{
			pThisSessionPtr->register_cache_shared_core(pThisSessionPtr);

			//m_pSendEvent->m_registerSendEvent.SetIocpObject(pThisSessionPtr);

			m_bConnectedNonAtomic = m_bConnectedNonAtomicForRecv = true;
			//std::atomic_thread_fence(std::memory_order_seq_cst);
			m_bConnected.store(true);
			// 컨텐츠 코드에서 오버로딩 해야함
			// 입장시 해야할 일

			OnConnected();

			// 수신 등록(낚싯대 던짐)
			RegisterRecv(pThisSessionPtr);
		}
	}

	bool Session::RegisterDisconnect()noexcept
	{
		auto pValid = shared_from_this();
		if (nullptr == pValid)
		{
			SocketUtils::Close(m_sessionSocket);
			GetService()->ReleaseSession(this);
			return false;
		}
		m_pDisconnectEvent->Init();
		m_pDisconnectEvent->SetIocpObject(std::move(pValid));

		if (false == SocketUtils::DisconnectEx(m_sessionSocket, m_pDisconnectEvent.get(), TF_REUSE_SOCKET, 0))
		{
			const int32 errorCode = ::WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				m_bConnectedNonAtomic = m_bConnectedNonAtomicForRecv = false;
				m_bConnected.store(false);
				
				//CancelIoEx(reinterpret_cast<HANDLE>(m_sessionSocket), NULL);
				SocketUtils::Close(m_sessionSocket);

				//HandleError(errorCode);
				m_pDisconnectEvent->ReleaseIocpObject();
				//m_pSendEvent->m_registerSendEvent.ReleaseIocpObject();
				GetService()->ReleaseSession(this);
				OnDisconnected();
				return false;
			}
		}
		return true;
	}

	void Session::ProcessDisconnect(const S_ptr<PacketSession>& pThisSessionPtr, c_int32 numofBytes_)noexcept
	{
		//CancelIoEx(reinterpret_cast<HANDLE>(m_sessionSocket), NULL);
		SocketUtils::Close(m_sessionSocket);
		//GetService()->ReleaseSession(std::move(const_cast<S_ptr<PacketSession>&>(pThisSessionPtr)));
		GetService()->ReleaseSession(this);
		OnDisconnected();
		//m_pSendEvent->m_registerSendEvent.ReleaseIocpObject();
		// 캐시 쉐어드 리셋 해야됌
	}

	void Session::RegisterRecv(const S_ptr<PacketSession>& pThisSessionPtr)noexcept
	{
		//std::atomic_thread_fence(std::memory_order_acquire);
		if (false == m_bConnectedNonAtomicForRecv)
		{
			Disconnect(L"");
			return;
		}

		m_pRecvEvent->Init();
		m_pRecvEvent->SetIocpObject(std::move(const_cast<S_ptr<PacketSession>&>(pThisSessionPtr)));

		WSABUF wsaBuf{ static_cast<const ULONG>(m_pRecvBuffer->FreeSize()),reinterpret_cast<char* const>(m_pRecvBuffer->WritePos()) };
		//DWORD numOfBytes;
		DWORD flags = 0;
		if (SOCKET_ERROR == ::WSARecv(m_sessionSocket, &wsaBuf, 1, NULL, &flags, m_pRecvEvent.get(), recv_callback))
		{
			const int32 errorCode = ::WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				//m_bConnectedNonAtomic = m_bConnectedNonAtomicForRecv = false;
				//m_bConnected.store(false);

				//CancelIoEx(reinterpret_cast<HANDLE>(m_sessionSocket), NULL);
				//SocketUtils::Close(m_sessionSocket);

				HandleError(errorCode);
				//m_pSendEvent->m_registerSendEvent.ReleaseIocpObject();
				//GetService()->ReleaseSession(this);
				m_pRecvEvent->ReleaseIocpObject();
				//reset_cache_shared();
			}
		}
	}

	void Session::ProcessRecv(const S_ptr<PacketSession>& pThisSessionPtr, c_int32 numofBytes_)noexcept
	{
		if (0 == numofBytes_)
		{
			Disconnect(L"Recv 0");
			return;
		}

		if (false == m_pRecvBuffer->OnWrite(numofBytes_)) [[unlikely]]
		{
			Disconnect(L"OnWrite Overflow");
			return;
		}
		const int32 dataSize = m_pRecvBuffer->DataSize(); // 더 읽어야할 데이터 w - r
		// 컨텐츠 쪽에서 오버로딩 해야함

		const RecvStatus recvStatus = static_cast<PacketSession* const>(this)->PacketSession::OnRecv(m_pRecvBuffer->ReadPos(), dataSize, pThisSessionPtr);

		if (false == recvStatus.bIsOK || recvStatus.processLen < 0 || dataSize < recvStatus.processLen || false == m_pRecvBuffer->OnRead(recvStatus.processLen))
		{
			Disconnect(L"OnRecv Overflow");
			return;
		}

		m_pRecvBuffer->Clear(); // 커서 정리

		RegisterRecv(pThisSessionPtr);
	}

	void Session::RegisterSend(const S_ptr<PacketSession>& pThisSessionPtr)noexcept
	{
		if (false == IsConnected()) 
		{
			Disconnect(L"");
			//m_pSendEvent->m_registerSendEvent.ReleaseIocpObject();
			return;
		}

		m_pSendEvent->Init();
		m_pSendEvent->SetIocpObject(std::move(const_cast<S_ptr<PacketSession>&>(pThisSessionPtr)));
		//m_pSendEvent->SetIocpObject(shared_from_this());
		thread_local Vector<S_ptr<SendBuffer>> sendBuffer;
		//auto& sendBuffer = m_pSendEvent->sendBuffer;
		sendBuffer.clear();
		m_sendQueue.try_flush_single(sendBuffer);

		//int32 writeSize = 0;
		// 원기옥모아서한방에 보낸다
		Vector<WSABUF> wsaBufs;
		wsaBufs.reserve(sendBuffer.size());
		for (const auto& sb : sendBuffer)
		{
			wsaBufs.emplace_back(static_cast<const ULONG>(sb->WriteSize()), reinterpret_cast<char* const>(sb->Buffer()));
			//writeSize += sb->WriteSize();
		}
		//DWORD numOfBytes;
		if (SOCKET_ERROR == ::WSASend(m_sessionSocket, wsaBufs.data(), static_cast<const DWORD>(wsaBufs.size()), NULL, 0, m_pSendEvent.get(), send_callback))
		{
			const int32 errorCode = ::WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{

				//m_bConnectedNonAtomic = m_bConnectedNonAtomicForRecv = false;
				//m_bIsSendRegistered.store(false);

				//CancelIoEx(reinterpret_cast<HANDLE>(m_sessionSocket), NULL);
				//SocketUtils::Close(m_sessionSocket);

				HandleError(errorCode);
				m_pSendEvent->ReleaseIocpObject();
				//GetService()->ReleaseSession(this);
				//m_pSendEvent->m_registerSendEvent.ReleaseIocpObject();
				//reset_cache_shared();
			}
		}
	}

	void Session::ProcessSend(const S_ptr<PacketSession>& pThisSessionPtr, c_int32 numofBytes_)noexcept
	{
		if (0 == numofBytes_)
		{
			//m_pSendEvent->m_registerSendEvent.ReleaseIocpObject();
			Disconnect(L"Send 0");
			return;
		}
		//m_pSendEvent->SetRegisterSendEvent(pThisSessionPtr);
		//m_pSendEvent->m_registerSendEvent.SetIocpObject(pThisSessionPtr);

		OnSend(numofBytes_);

		m_bIsSendRegistered.store(false, std::memory_order_release);

		if (!m_sendQueue.empty_single() && false == m_bIsSendRegistered.exchange(true, std::memory_order_relaxed))
			RegisterSend(pThisSessionPtr);
	}

	void Session::HandleError(c_int32 errorCode)
	{
		Disconnect(L"HandleError");
		switch (errorCode)
		{
		case WSAECONNRESET:
		case WSAECONNABORTED:
			//Disconnect(L"HandleError");
			break;
		default:
			// TODO 로그찍기
			//LOG_MSG(std::format(L"Handle Error: {}", errorCode));
			break;
		}
	}

	void RecvCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped, DWORD dwFlags)
	{
		auto pSession = static_pointer_cast<PacketSession>(static_cast<RecvEvent* const>(lpOverlapped)->PassIocpObject());
		pSession->ProcessRecv(pSession, dwNumberOfBytesTransfered);
	}

	void SendCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped, DWORD dwFlags)
	{
		auto pSession = static_pointer_cast<PacketSession>(static_cast<RecvEvent* const>(lpOverlapped)->PassIocpObject());
		pSession->ProcessSend(pSession, dwNumberOfBytesTransfered);
	}
}
