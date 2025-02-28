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

	m_aID = Server()->SnapNewId();

	Server()->SnapNewId();
	GameWorld()->InsertEntity(this);
}

void CHeadItem::Reset()
{
	Server()->SnapFreeId(m_aID);
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

	m_TeamMask = pOwner->TeamMask();
	m_Pos = pOwner->GetPos();
	m_aPos = vec2(m_Pos.x, m_Pos.y - 70.f);
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

	int Size = Server()->IsSixup(SnappingClient) ? 4 * 4 : sizeof(CNetObj_Pickup);
	CNetObj_Pickup *pPickup = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_aID, Size));
	if(!pPickup)
		return;

	pPickup->m_X = round_to_int(m_aPos.x);
	pPickup->m_Y = round_to_int(m_aPos.y);
	if(Server()->IsSixup(SnappingClient))
	{
		if(m_Type == 8)
		{
			pPickup->m_Type = POWERUP_WEAPON;
			pPickup->m_Subtype = WEAPON_GUN;
		}
		else if(m_Type == 9)
		{
			pPickup->m_Type = POWERUP_WEAPON;
			pPickup->m_Subtype = WEAPON_SHOTGUN;
		}
		else if(m_Type == 10)
		{
			pPickup->m_Type = POWERUP_WEAPON;
			pPickup->m_Subtype = WEAPON_GRENADE;
		}
		else if(m_Type == 11)
		{
			pPickup->m_Type = POWERUP_WEAPON;
			pPickup->m_Subtype = WEAPON_LASER;
		}
		else
			pPickup->m_Type = m_Type;
		((int *)pPickup)[3] = 0;
	}
	else
	{
		if(m_Type == 8)
		{
			pPickup->m_Type = 2;
			pPickup->m_Subtype = WEAPON_GUN;
		}
		else if(m_Type == 9)
		{
			pPickup->m_Type = 2;
			pPickup->m_Subtype = WEAPON_SHOTGUN;
		}
		else if(m_Type == 10)
		{
			pPickup->m_Type = 2;
			pPickup->m_Subtype = WEAPON_GRENADE;
		}
		else if(m_Type == 11)
		{
			pPickup->m_Type = 2;
			pPickup->m_Subtype = WEAPON_LASER;
		}
		else
			pPickup->m_Type = m_Type;
	}
}
