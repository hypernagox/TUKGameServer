#include "pch.h"
#include "ContentsEntity.h"
#include "Horse.h"

ContentsEntity::ContentsEntity() noexcept
	:m_pHorse{ ServerCore::MakeShared<Horse>() }
{
}
