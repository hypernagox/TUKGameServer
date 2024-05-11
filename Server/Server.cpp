#include "pch.h"
#include "Service.h"
#include "IocpCore.h"
#include "c2s_PacketHandler.h"
#include "ClientSession.h"
#include "Horse.h"
#include "Chess.h"
#include "SocketUtils.h"
#include "Broadcaster.h"
#include "SimplePacket.h"
#include "NPC.h"

//std::array<std::shared_ptr<ClientSession>, 15000> g_session_arr;

constexpr const int dx[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
constexpr const int dy[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };

void pro_recv(int id)
{
	//Mgr(ThreadMgr)->InitTLS();
	//const auto& p = g_session_arr[id];
	//while (true) {
	//	const int res = recv(p->GetSocket(), (char*)p->m_pRecvBuffer->ReadPos(), 100, 0);
	//	if (0 == res)
	//	{
	//		g_session_arr[p->GetSessionID()].reset();
	//		return;
	//	}
	//	else
	//	{
	//		p->m_pRecvBuffer->OnWrite(res);
	//		const char dataSize = p->m_pRecvBuffer->DataSize();
	//
	//		const ServerCore::RecvStatus recvStatus = static_cast<ServerCore::PacketSession* const>(p.get())->ServerCore::PacketSession::OnRecv(p->m_pRecvBuffer->ReadPos(), dataSize, p);
	//
	//		p->m_pRecvBuffer->OnRead(recvStatus.processLen);
	//		p->m_pRecvBuffer->Clear();
	//	}
	//}
}

using namespace ServerCore;

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

	Mgr(CoreGlobal)->Init();
	ServerCore::c2s_PacketHandler::Init();
	 
	ServerCore::Broadcaster::RegisterHuristicFunc([](const IocpEntity* const a, const IocpEntity* const b)
		{
			if constexpr (0 == VIEW_RANGE)
				return true;
			const auto& aa = a->GetContentsEntity()->GetHorse();
			const auto& bb = b->GetContentsEntity()->GetHorse();
			
			const int dist = (aa->m_xPos - bb->m_xPos) * (aa->m_xPos - bb->m_xPos) +
				(aa->m_yPos - bb->m_yPos) * (aa->m_yPos - bb->m_yPos);
			
			const auto types = (bool)a->GetObjectType() + (bool)b->GetObjectType();
			if (1 == types)
			{
				const bool bFlag = dist <= NPC_RANGE * NPC_RANGE;
				//const bool bFlag = true;
				const auto aa = a->ObjectCast<NPC>(1);
				const auto bb = b->ObjectCast<NPC>(1);
				if (aa && bFlag) {
					if (aa->ExecuteTimer())
						aa->m_time = std::chrono::steady_clock::now();
					//std::cout << aa->GetContentsEntity()->getName() << std::endl;
				}
				else if (bb && bFlag) {
					if (bb->ExecuteTimer())
						bb->m_time = std::chrono::steady_clock::now();
					//std::cout << bb->GetContentsEntity()->getName() << std::endl;
				}
			}
			

			return dist <= VIEW_RANGE * VIEW_RANGE;
		});

	ServerCore::Broadcaster::RegisterInPacketFunc([](const S_ptr<IocpEntity>& p)
		{
			ServerCore::SC_ADD_PLAYER_PACKET add_packet;
			const auto& pHorse = p->GetContentsEntity()->GetHorse();
			add_packet.id = (int)p->GetObjectID();
			strcpy_s(add_packet.name, p->GetContentsEntity()->getName());
			add_packet.pkt_size = sizeof(add_packet);
			add_packet.pkt_id = SC_ADD_PLAYER;
			add_packet.x = (short)p->GetContentsEntity()->GetHorse()->m_xPos;
			add_packet.y = (short)p->GetContentsEntity()->GetHorse()->m_yPos;
			return add_packet.MakeSendBuffer();
		});

	ServerCore::Broadcaster::RegisterOutPacketFunc([](const S_ptr<IocpEntity>& p)
		{
			ServerCore::SC_REMOVE_PLAYER_PACKET remove_packet;
			const auto& pHorse = p->GetContentsEntity()->GetHorse();
			remove_packet.id = (int)p->GetObjectID();
			remove_packet.pkt_size = sizeof(remove_packet);
			remove_packet.pkt_id = SC_REMOVE_PLAYER;
			return remove_packet.MakeSendBuffer();
		});

	for (auto& board : Horse::g_board | std::views::join)
	{
		board = ServerCore::MakeShared<Chess>();
		board->StartExecute(board);
		
	}
	for (int i = 0; i < W_HEIGHT/ SECTOR_SIZE; ++i)
	{
		for (int j = 0; j < W_WIDTH/ SECTOR_SIZE; ++j)
		{
			Horse::g_board[i][j]->m_adjVector.emplace_back(Horse::g_board[i][j].get());
			for (int k = 0; k < 8; ++k)
			{
				const int ny = i + dy[k];
				const int nx = j + dx[k];
				if (ny < 0 || nx < 0 || ny >= W_HEIGHT / SECTOR_SIZE || nx >= W_WIDTH / SECTOR_SIZE)continue;
				//Horse::g_board[i][j]->m_adjSector[k] = Horse::g_board[ny][nx].get();
				Horse::g_board[i][j]->m_adjVector.emplace_back(Horse::g_board[ny][nx].get());
			}
			//for (int m = 0; m < 2; ++m)
			////static bool flag = true;
			////if(flag)
			//{
			//	//flag = false;
			//	auto npc = ServerCore::MakeShared<NPC>();
			//	//npc->register_cache_shared_core(npc);
			//	npc->InitTimer(npc, 1000);
			//	const auto& pHorse = npc->GetContentsEntity()->GetHorse();
			//	pHorse->m_xPos = rand() % W_WIDTH;;
			//	pHorse->m_yPos = rand() % W_HEIGHT;
			//	//pHorse->m_xPos = 40;
			//	//pHorse->m_yPos = 40;
			//	strcpy(npc->GetContentsEntity()->getName(), "NPC");
			//	//auto m = npc->GetContentsEntity();
			//	//Horse::g_board[pHorse->m_xPos / 8][pHorse->m_yPos / 8]->m_objects.AddItem(npc->GetObjectID(), npc->GetContentsEntity());
			//	//const auto p = Horse::g_board[pHorse->m_xPos / 8][pHorse->m_yPos / 8]->m_objects.FindItem(5);
			//	Horse::g_board[pHorse->m_yPos/8][pHorse->m_xPos/8]->Enter(std::move(npc));
			//}
		}
		
	}
	for (int i = 0; i < 200000; ++i)
	{
		{
			//flag = false;
			auto npc = ServerCore::MakeShared<NPC>();
			//npc->register_cache_shared_core(npc);
			npc->InitTimer(npc, 1000);
			const auto& pHorse = npc->GetContentsEntity()->GetHorse();
			pHorse->m_xPos = rand() % W_WIDTH;;
			pHorse->m_yPos = rand() % W_HEIGHT;
			//pHorse->m_xPos = 40;
			//pHorse->m_yPos = 40;
			strcpy(npc->GetContentsEntity()->getName(), std::format("NPC {}",npc->GetObjectID()).c_str());
			//auto m = npc->GetContentsEntity();
			//Horse::g_board[pHorse->m_xPos / 8][pHorse->m_yPos / 8]->m_objects.AddItem(npc->GetObjectID(), npc->GetContentsEntity());
			//const auto p = Horse::g_board[pHorse->m_xPos / 8][pHorse->m_yPos / 8]->m_objects.FindItem(5);
			Horse::g_board[pHorse->m_yPos / 8][pHorse->m_xPos / 8]->Enter(std::move(npc));
		}
	}
	//Horse::g_board = ServerCore::MakeShared<Chess>();
	//Horse::g_board->StartExecute(Horse::g_board);

	//const auto pRoom = std::make_shared<Chess>();
	//Horse::g_board = pRoom;
	//pRoom->register_cache_shared_core(pRoom);

	//Horse::g_board->RegisterHeartBeat();

#if defined(MULTI_IOCP) || defined(SINGLE_IOCP) || defined(OVERLAPPED_IO)
	 const auto pServerService = ServerCore::MakeShared<ServerCore::ServerService>
	 	(
	 		  Mgr(CoreGlobal)->GetIocpCore()
	 		, ServerCore::NetAddress{ L"0.0.0.0", 4000 }
	 		, ServerCore::MakePoolShared<ClientSession>
	 		, 15000
	 	);
	 
	 ASSERT_CRASH(pServerService->Start());
	 
	 
	 
	 Mgr(ThreadMgr)->Launch(pServerService);
#endif

#if defined(NON_BLOCK) || defined(SOCK_TH)
	SOCKET m_socket = ServerCore::SocketUtils::CreateSocket();
	auto server_addr = ServerCore::NetAddress{ L"0.0.0.0", 4000 };
	if (INVALID_SOCKET == m_socket)
		return false;
	if (false == ServerCore::SocketUtils::SetReuseAddress(m_socket, true))
		return false;
	if (false == ServerCore::SocketUtils::SetLinger(m_socket, 0, 0))
		return false;
	if (false == ServerCore::SocketUtils::Bind(m_socket, server_addr))
		return false;
	if (false == ServerCore::SocketUtils::Listen(m_socket))
		return false;
	std::vector<std::thread> th;
	std::thread accept_th{ [m_socket,&server_addr,&th]() {
		while (true) {
		sockaddr_in address = server_addr.GetSockAddr();
		int addrlen = sizeof(address);
		const SOCKET socket = accept(m_socket, (sockaddr*)&address, (socklen_t*)&addrlen);
		u_long mode = 1;
#if defined(NON_BLOCK)
		ioctlsocket(socket, FIONBIO, &mode);
#endif
		auto pClient = ServerCore::MakePoolShared<ClientSession>(socket);
		const auto id = pClient->GetSessionID();
		g_session_arr[id] = std::move(pClient);
#if defined(SOCK_TH)
		th.emplace_back(pro_recv, id);
#endif
		}
	} };
#endif
#if defined(NON_BLOCK)
	while (true)
	{
		for (const auto& p : g_session_arr)
		{
			if (!p)continue;
			const int res = recv(p->GetSocket(), (char*)p->m_pRecvBuffer->ReadPos(), 100, 0);
			if (0 > res)continue;
			if (0 == res)
			{
				g_session_arr[p->GetSessionID()].reset();
			}
			else
			{
				p->m_pRecvBuffer->OnWrite(res);
				const char dataSize = p->m_pRecvBuffer->DataSize();
	
				const ServerCore::RecvStatus recvStatus = static_cast<ServerCore::PacketSession* const>(p.get())->ServerCore::PacketSession::OnRecv(p->m_pRecvBuffer->ReadPos(), dataSize, p);
	
				p->m_pRecvBuffer->OnRead(recvStatus.processLen);
				p->m_pRecvBuffer->Clear();
			}
		}
	}
#endif
#if defined(NON_BLOCK) || defined(SOCK_TH)
	accept_th.join();
	for (auto& t : th) {
		if (t.joinable())
			t.join();
	}
#endif

	for (auto& board : Horse::g_board | std::views::join)
	{

		board->StopExecute();
		board.reset();
	}
	//Horse::g_board->StopExecute();
	//Horse::g_board.reset();
	//google::protobuf::ShutdownProtobufLibrary();
}