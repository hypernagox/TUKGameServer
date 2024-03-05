#pragma once
#include "CScene.h"

class CObject;
class CTexture;
class CLayer;
class CTileLayer;
class CPlayer;
class CCthulhuEye;

class CScene_Stage
	:public CScene
{
	friend class CDebugMgr;
	friend class CCollisionMgr;
	friend class CMiniMap;
public:
	CScene_Stage();
	virtual ~CScene_Stage();
private:
	vector<unique_ptr<CLayer>> m_vecLayer;
	vector<unique_ptr<CTileLayer>> m_vecTileLayer;
	Vec2 m_vRes = {};
	CObject*							m_pPlayer = {};
	HDC	m_hSceneThreadDC[THREAD::END + 1];
	HBITMAP	m_hSceneThreadBit[THREAD::END + 1];

public: 
	void RegisterPlayer(CObject* const _pPlayer) { m_pPlayer = _pPlayer; }
	CObject* GetPlayer()const { return m_pPlayer; }
	CPlayer* GetPlayerCast()const { return reinterpret_cast<CPlayer*>(m_pPlayer); }
	auto& GetPlayerWeapon() { return m_vecObj[etoi(GROUP_TYPE::PLAYER_WEAPON)]; }
public:
	virtual void Reset()override;
	virtual void render(HDC _dc)override;
	virtual void Enter()override;
	virtual void Exit()override;
public:
	void AddTileLayer(CTileLayer* const _pTileLayer);

};
