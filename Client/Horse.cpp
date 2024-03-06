#include "pch.h"
#include "Horse.h"
#include "CResMgr.h"
#include "CKeyMgr.h"

Horse::Horse()
{
	SetPos({ 50,50 });
	SetScale({ 64,64 });
}

Horse::~Horse()
{
}

void Horse::update()
{
	const auto curPos = GetPos();
	Vec2 addPos = {};
	if (KEY_HOLD(VK_LEFT))
	{
		addPos += { -10.f,0 };
	}
	if (KEY_HOLD(VK_DOWN))
	{
		addPos += { 0.f,10.f };
	}
	if (KEY_HOLD(VK_RIGHT))
	{
		addPos += { 10.f,0 };
	}
	if (KEY_HOLD(VK_UP))
	{
		addPos += { 0.f,-10.f };
	}
	SetPos(curPos + addPos * DT * 10.f);
}

void Horse::component_update()
{
	CObject::component_update();
}

void Horse::render(HDC _dc) const
{
	Mgr(CResMgr)->renderImg(_dc, m_pImg, this, { 0,0 }, { 64,64 }, false);
}
