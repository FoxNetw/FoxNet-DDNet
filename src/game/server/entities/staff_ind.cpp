#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include "character.h"
#include "staff_ind.h"

CStaffInd::CStaffInd(CGameWorld *pGameWorld, vec2 Pos, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_STAFF_IND, Pos)
{
	m_Owner = Owner;

	m_Dist = 0.f;
	m_BallFirst = true;
	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_TeamMask = pOwnerChar ? pOwnerChar->TeamMask() : CClientMask();

	for (int i = 0; i < NUM_IDS; i++)
		m_aIds[i] = Server()->SnapNewId();
	std::sort(std::begin(m_aIds), std::end(m_aIds));
	GameWorld()->InsertEntity(this);
}

void CStaffInd::Reset()
{
	for(int i = 0; i < NUM_IDS; i ++) 
   		Server()->SnapFreeId(m_aIds[i]);
	GameWorld()->RemoveEntity(this);
}

void CStaffInd::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if(!pOwner || !pOwner->GetPlayer()->m_StaffInd)
	{
		Reset();
		return;
	}

	m_TeamMask = pOwner->TeamMask();
	m_Pos = pOwner->GetPos();
	m_aPos[ARMOR] = vec2(m_Pos.x, m_Pos.y - 70.f);

	if(m_BallFirst)
	{
		m_Dist += 0.9f;
		if (m_Dist > 25.f)
			m_BallFirst = false;
	}
	else
	{
		m_Dist -= 0.9f;
		if (m_Dist < -25.f)
			m_BallFirst = true;
	}

	m_aPos[BALL] = vec2(m_Pos.x + m_Dist, m_aPos[ARMOR].y);
}

void CStaffInd::Snap(int SnappingClient)
{	

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if(pOwner && pOwner->IsPaused())
		return;

	if(GameServer()->GetPlayerChar(SnappingClient) && pOwner)
		if(!GameServer()->GetPlayerChar(m_Owner)->CanSnapCharacter(SnappingClient))
			return;

	if(GameServer()->GetPlayerChar(m_Owner)->GetPlayer()->m_Vanish && SnappingClient != GameServer()->GetPlayerChar(m_Owner)->GetPlayer()->GetCid() && SnappingClient != -1)
		if(!GameServer()->m_apPlayers[SnappingClient]->m_Vanish && Server()->GetAuthedState(SnappingClient) < AUTHED_OWNER)
			return;

	int Size = Server()->IsSixup(SnappingClient) ? sizeof(CNetObj_DDNetPickup) : 4 * 4;
	CNetObj_DDNetPickup *pArmor = static_cast<CNetObj_DDNetPickup *>(Server()->SnapNewItem(NETOBJTYPE_DDNETPICKUP, m_aIds[ARMOR], Size));
	if (!pArmor)
		return;

	pArmor->m_X = round_to_int(m_aPos[ARMOR].x);
	pArmor->m_Y = round_to_int(m_aPos[ARMOR].y);

	if(Server()->IsSixup(SnappingClient))
		pArmor->m_Type = POWERUP_ARMOR;
	else
	{
		pArmor->m_Type = POWERUP_ARMOR;
		((int *)pArmor)[3] = 0;
	}

	// m_ID is created before m_aID is created, means that id is lower and we can simply use it to make the ball behind
	CNetObj_DDNetLaser *pLaser = static_cast<CNetObj_DDNetLaser *>(Server()->SnapNewItem(NETOBJTYPE_DDNETLASER, m_BallFirst ? m_aIds[BALL_FRONT] : m_aIds[BALL], sizeof(CNetObj_DDNetLaser)));
	if(!pLaser)
		return;

	pLaser->m_ToX = round_to_int(m_aPos[BALL].x);
	pLaser->m_ToY = round_to_int(m_aPos[BALL].y);
	pLaser->m_FromX = round_to_int(m_aPos[BALL].x);
	pLaser->m_FromY = round_to_int(m_aPos[BALL].y);
	pLaser->m_StartTick = Server()->Tick();
}


