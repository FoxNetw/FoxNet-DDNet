#include "character.h"
#include "head_powerup.h"
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/teams.h>

CHeadItem::CHeadItem(CGameWorld *pGameWorld, vec2 Pos, int Owner, int Type) :
	CEntity(pGameWorld, CGameWorld::ENTTYPE_PICKUP, Pos)
{
	m_Owner = Owner;

	// Type of Entity
	m_Type = Type;

	m_Dist = 0.f;
	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_TeamMask = pOwnerChar ? pOwnerChar->TeamMask() : CClientMask();

	m_aId = Server()->SnapNewId();

	Server()->SnapNewId();
	GameWorld()->InsertEntity(this);
}

void CHeadItem::Reset()
{
	Server()->SnapFreeId(m_aId);
	GameWorld()->RemoveEntity(this);
}

void CHeadItem::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner || !pOwner->m_HeadItem)
	{
		Reset();
		return;
	}

	vec2 Vel = pOwner->Core()->m_Vel / 2.0f;

	m_TeamMask = pOwner->TeamMask();
	m_Pos = pOwner->GetPos();
	m_aPos = vec2(m_Pos.x + (int)Vel.x, m_Pos.y - 70.f + (int)Vel.y);
}

void CHeadItem::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if(pOwner && pOwner->IsPaused())
		return;

	if(GameServer()->GetPlayerChar(SnappingClient) && pOwner)
		if(!GameServer()->GetPlayerChar(m_Owner)->CanSnapCharacter(SnappingClient))
			return;

	int Type = m_Type;
	int SubType = Type - 7;
	if(m_Type == 8)
		Type = POWERUP_WEAPON;
	else if(m_Type == 9)
		Type = POWERUP_WEAPON;
	else if(m_Type == 10)
		Type = POWERUP_WEAPON;
	else if(m_Type == 11)
		Type = POWERUP_WEAPON;

	if(GameServer()->GetClientVersion(SnappingClient) >= VERSION_DDNET_ENTITY_NETOBJS)
	{
		CNetObj_DDNetPickup *pPickup = Server()->SnapNewItem<CNetObj_DDNetPickup>(m_aId);
		if(!pPickup)
			return;

		pPickup->m_X = (int)(m_aPos.x);
		pPickup->m_Y = (int)(m_aPos.y);
		pPickup->m_Type = Type;
		pPickup->m_Subtype = SubType;
	}
	else
	{
		CNetObj_Pickup *pPickup = Server()->SnapNewItem<CNetObj_Pickup>(m_aId);
		if(!pPickup)
			return;

		pPickup->m_X = (int)(m_aPos.x);
		pPickup->m_Y = (int)(m_aPos.y);

		pPickup->m_Type = Type;
		if(GameServer()->GetClientVersion(SnappingClient) < VERSION_DDNET_WEAPON_SHIELDS)
		{
			if(Type >= POWERUP_ARMOR_SHOTGUN && Type <= POWERUP_ARMOR_LASER)
			{
				pPickup->m_Type = POWERUP_ARMOR;
			}
		}
		pPickup->m_Subtype = SubType;
	}
}
