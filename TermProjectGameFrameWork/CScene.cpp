#include "pch.h"
#include "CScene.h"
#include "CObject.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::AddObject(CObject* const _pObj, GROUP_TYPE _eType)
{
	m_vecObj[(UINT)_eType].emplace_back(_pObj);
}

void CScene::DeleteGroup(GROUP_TYPE _eTarget)
{
	m_vecObj[etoi(_eTarget)].clear();
}

void CScene::Reset()
{
	for (auto& i : m_vecObj)
	{
		i.clear();
	}
}

void CScene::render(HDC _dc)
{
	for (auto& vecObj : m_vecObj)
	{
		const auto vecPtr = vecObj.data();
		for (size_t i = 0, size = vecObj.size(); i < size;)
		{
			if (vecPtr[i]->IsDead())
			{
				vecPtr[i].swap(vecObj.back());
				vecObj.pop_back();
			}
			else
			{
				vecPtr[i]->render(_dc);
				++i;
			}
		}
	}
}

void CScene::component_update() const
{
	for (const auto& vecObj : m_vecObj)
	{
		const auto vecPtr = vecObj.data();
		for (size_t i = 0, size = vecObj.size(); i < size; ++i)
		{
			vecPtr[i]->component_update();
		}
	}
}

void CScene::update()
{
	for (const auto& vecObj : m_vecObj)
	{
		const auto vecPtr = vecObj.data();
		for (size_t i = 0, size = vecObj.size(); i < size; ++i)
		{
			if (vecPtr[i]->IsDead())
			{
				continue;
			}
			vecPtr[i]->update();
		}
	}
}

void CScene::Enter()
{
}

void CScene::Exit()
{
}
