#include "character.h"
#include "ability_ind.h"
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/teams.h>

CAbilityInd::CAbilityInd(CGameWorld *pGameWorld, vec2 Pos, int Owner) :
	CEntity(pGameWorld, CGameWorld::ENTTYPE_ABILITY_IND, Pos)
{
	m_Owner = Owner;

	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_TeamMask = pOwnerChar ? pOwnerChar->TeamMask() : CClientMask();

	m_aId = Server()->SnapNewId();
	GameWorld()->InsertEntity(this);
}

void CAbilityInd::Reset()
{
	Server()->SnapFreeId(m_aId);
	GameWorld()->RemoveEntity(this);
}

void CAbilityInd::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if(!pOwner || !pOwner->m_AbilityInd)
	{
		Reset();
		return;
	}

	vec2 Vel = pOwner->Core()->m_Vel / 2.0f;

	m_TeamMask = pOwner->TeamMask();
	m_Pos = pOwner->GetPos();
	m_aPos = vec2(m_Pos.x + (int)Vel.x, m_Pos.y - 45.f + (int)Vel.y);
}

void CAbilityInd::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if(pOwner && pOwner->IsPaused())
		return;

	if(GameServer()->GetPlayerChar(SnappingClient) && pOwner)
	{
		if(!GameServer()->GetPlayerChar(m_Owner)->CanSnapCharacter(SnappingClient))
			return;

		if(!GameServer()->GetPlayerChar(SnappingClient)->GetPlayer()->m_AbilityIndicator)
			return;
	}

	if(GameServer()->GetClientVersion(SnappingClient) >= VERSION_DDNET_MULTI_LASER)
	{
		CNetObj_DDNetLaser *pObj = Server()->SnapNewItem<CNetObj_DDNetLaser>(m_aId);
		if(!pObj)
			return;

		pObj->m_ToX = round_to_int(m_aPos.x);
		pObj->m_ToY = round_to_int(m_aPos.y);
		pObj->m_FromX = round_to_int(m_aPos.x);
		pObj->m_FromY = round_to_int(m_aPos.y);
		pObj->m_StartTick = Server()->Tick();
		pObj->m_Owner = m_Owner;
		pObj->m_Type = 0;
	}
	else
	{
		CNetObj_Laser *pObj = Server()->SnapNewItem<CNetObj_Laser>(m_aId);
		if(!pObj)
			return;

		pObj->m_X = round_to_int(m_aPos.x);
		pObj->m_Y = round_to_int(m_aPos.y);
		pObj->m_FromX = round_to_int(m_aPos.x);
		pObj->m_FromY = round_to_int(m_aPos.y);
		pObj->m_StartTick = Server()->Tick();
	}
}
