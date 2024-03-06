#pragma once

enum class KEY_STATE
{
	NONE,	
	TAP, 
	HOLD, 
	AWAY, 
};

struct tKeyInfo
{
	const int VK = -1;
	KEY_STATE		eState = KEY_STATE::NONE;
	bool			bPrevPush = false;
	tKeyInfo(const int _VKval) noexcept :VK{ _VKval }, eState{ KEY_STATE::NONE }, bPrevPush{ false } {}
	operator bool()const noexcept{ return -1 != VK; }
};

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

class CKeyMgr
	:public Singleton<CKeyMgr>
{
	friend LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	friend class Singleton;
	CKeyMgr();
	~CKeyMgr();
private:
	unordered_map<int, tKeyInfo> m_mapKey;
	Vec2			m_vCurMousePos = {};
	POINT			m_ptCurMousePos = {};
	short			m_wheelDelta=0;
public:
	void init();
	void update();
public:
	const tKeyInfo& GetKeyInfo(const int _VKval) noexcept { return m_mapKey.try_emplace(_VKval, _VKval).first->second; }
	const KEY_STATE GetKeyState(const int _VKval) noexcept { return GetKeyInfo(_VKval).eState; }

	Vec2	GetMousePos()const { return m_vCurMousePos; }
	POINT   GetMousePosPt()const { return m_ptCurMousePos; }
	void ReSetWheel() { m_wheelDelta = 0; }
	bool GetMouseWheelUp()const { return m_wheelDelta > 0; }
	bool GetMouseWheelDown()const { return m_wheelDelta < 0; }
};
