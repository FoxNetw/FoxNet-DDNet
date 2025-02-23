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

	if(!pOwner || pOwner->m_HeadItem < -1)
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

	int Size = Server()->IsSixup(SnappingClient) ? 4 * 4 : sizeof(CNetObj_Pickup);
	CNetObj_Pickup *pPickup = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_aID, Size));
	if(!pPickup)
		return;

	pPickup->m_X = round_to_int(m_aPos.x);
	pPickup->m_Y = round_to_int(m_aPos.y);
	if(Server()->IsSixup(SnappingClient))
	{
		pPickup->m_Type = m_Type;
		((int *)pPickup)[3] = 0;
	}
	else
		pPickup->m_Type = m_Type;

}
