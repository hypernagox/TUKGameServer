#pragma once
#include "IocpObject.h"

class Horse;
class PacketSession;

class ContentsEntity
{
public:
	ContentsEntity()noexcept;
	//~ContentsEntity() { std::cout << "Entity Destroy\n"; }
public:
	const std::shared_ptr<Horse>& GetHorse()const noexcept { return m_pHorse; }
	const char* const getName()const noexcept { return m_strName; }
	char* const getName()noexcept { return m_strName; }
private:
	const std::shared_ptr<Horse> m_pHorse;
	char m_strName[20];
public:
	unsigned int m_lastMoveTime = 0;
};

