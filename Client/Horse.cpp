#include "Horse.h"
#include "CResMgr.h"

void Horse::render(HDC _dc) const
{
	Mgr(CResMgr)->renderImg(_dc, m_pImg, { 0,0 }, { 64,64 }, { 0,0 }, { 64,64 });
}
