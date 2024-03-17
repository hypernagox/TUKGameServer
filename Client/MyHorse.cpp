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
	NetHelper::c2s_KEY pkt;
	pkt.moveUserID = NetMgr(NetworkMgr)->GetSessionID();

	if (KEY_TAP(VK_LEFT))
	{
		pkt.VK = VK_LEFT;
		Send(pkt);
	}
	if (KEY_TAP(VK_DOWN))
	{
		pkt.VK = VK_DOWN;
		Send(pkt);
	}
	if (KEY_TAP(VK_RIGHT))
	{
		pkt.VK = VK_RIGHT;
		Send(pkt);
	}
	if (KEY_TAP(VK_UP))
	{
		pkt.VK = VK_UP;
		Send(pkt);
	}
}

void MyHorse::component_update()
{
}
