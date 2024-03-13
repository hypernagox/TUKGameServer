#include "pch.h"
#include "MyHorse.h"
#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "Chess.h"
#include "SimplePacket.h"
#include "NetworkMgr.h"

MyHorse::MyHorse()
{
}

MyHorse::~MyHorse()
{
}

void MyHorse::update()
{
	if (KEY_TAP(VK_LEFT))
	{
		NetHelper::c2s_KEY pkt;
		pkt.VK = VK_LEFT;
		NetMgr(NetworkMgr)->Send(pkt.MakeSendBuffer());
	}
	if (KEY_TAP(VK_DOWN))
	{
		NetHelper::c2s_KEY pkt;
		pkt.VK = VK_DOWN;
		NetMgr(NetworkMgr)->Send(pkt.MakeSendBuffer());
	}
	if (KEY_TAP(VK_RIGHT))
	{
		NetHelper::c2s_KEY pkt;
		pkt.VK = VK_RIGHT;
		NetMgr(NetworkMgr)->Send(pkt.MakeSendBuffer());
	}
	if (KEY_TAP(VK_UP))
	{
		NetHelper::c2s_KEY pkt;
		pkt.VK = VK_UP;
		NetMgr(NetworkMgr)->Send(pkt.MakeSendBuffer());
	}
}

void MyHorse::component_update()
{
}
