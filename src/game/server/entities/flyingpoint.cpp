#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include "character.h"
#include "flyingpoint.h"

CFlyingPoint::CFlyingPoint(CGameWorld *pGameWorld, vec2 Pos, int To, int Owner, vec2 InitialVel, vec2 ToPos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_FLYINGPOINT, Pos)
{
	m_Pos = Pos;
	m_PrevPos = Pos;
	m_InitialVel = InitialVel;
	m_Owner = Owner;
	m_To = To;
	m_ToPos = ToPos;
	m_InitialAmount = 1.0f;
	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_TeamMask = pOwnerChar ? pOwnerChar->TeamMask() : CClientMask();
	GameWorld()->InsertEntity(this);
}

void CFlyingPoint::Reset()
{
	CCharacter *pChr = GameServer()->GetPlayerChar(m_To);
	vec2 Pos = pChr ? pChr->GetPos() : m_Pos;
	int ID = pChr ? m_To : m_Owner;
	GameServer()->CreateDeath(Pos, ID);
	GameWorld()->RemoveEntity(this);
}

void CFlyingPoint::Tick()
{
	vec2 ToPos = m_ToPos;
	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_TeamMask = pOwnerChar ? pOwnerChar->TeamMask() : CClientMask();

	if (m_To != -1)
	{
		CCharacter *pChr = GameServer()->GetPlayerChar(m_To);
		if (!pChr)
		{
			Reset();
			return;
		}

		ToPos = pChr->GetPos();
	}

	float Dist = distance(m_Pos, ToPos);
	if(Dist < 24.0f)
	{
		Reset();
		return;
	}

	vec2 Dir = normalize(ToPos - m_Pos);
	m_Pos += Dir*clamp(Dist, 0.0f, 16.0f) * (1.0f - m_InitialAmount) + m_InitialVel * m_InitialAmount;
	
	m_InitialAmount *= 0.98f;
	m_PrevPos = m_Pos;
}

void CFlyingPoint::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if(pOwner && pOwner->IsPaused())
		return;

	if(GameServer()->GetPlayerChar(SnappingClient) && pOwner)
		if(!GameServer()->GetPlayerChar(m_Owner)->CanSnapCharacter(SnappingClient))
			return;

	if(GameServer()->GetPlayerChar(m_Owner)->GetPlayer()->m_Vanish && SnappingClient != GameServer()->GetPlayerChar(m_Owner)->GetPlayer()->GetCid() && SnappingClient != -1)
		if(!GameServer()->m_apPlayers[SnappingClient]->m_Vanish && Server()->GetAuthedState(SnappingClient) < AUTHED_OWNER)
			return;

	CNetObj_Projectile *pObj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, GetId(), sizeof(CNetObj_Projectile)));
	if(pObj)
	{
		pObj->m_X = round_to_int(m_Pos.x);
		pObj->m_Y = round_to_int(m_Pos.y);
		pObj->m_VelX = 0;
		pObj->m_VelY = 0;
		pObj->m_StartTick = Server()->Tick();
		pObj->m_Type = WEAPON_HAMMER;
	}
}