#pragma once
#include "pch.h"

class CObject;

class CScene
{
	friend class CDebugMgr;
	friend class CCollisionMgr;
	friend class CMiniMap;
private:
	const auto& GetSceneObj()const { return m_vecObj; }
public:
	CScene();
	virtual ~CScene();
protected:
	vector<unique_ptr<CObject>>			m_vecObj[(UINT)GROUP_TYPE::END];
	wstring								m_strName;
public:
	void AddObject(CObject* const _pObj, GROUP_TYPE _eType);
	const vector<unique_ptr<CObject>>& GetGroupObject(GROUP_TYPE _eType)const { return m_vecObj[(UINT)_eType]; }
	vector<unique_ptr<CObject>>& GetUIGroup() { return m_vecObj[(UINT)GROUP_TYPE::UI]; }
	
public:
	void DeleteGroup(GROUP_TYPE _eTarget);
	virtual void Reset();


	virtual void render(HDC _dc);
	virtual void component_update()const;
	virtual void update();
	virtual void Enter();
	virtual void Exit();

public:
	void SetName(wstring_view _strName) { m_strName = _strName; }
	const wstring& GetName() { return m_strName; }
};
