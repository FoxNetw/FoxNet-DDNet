#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include "character.h"
#include "rotating_ball.h"

CRotatingBall::CRotatingBall(CGameWorld *pGameWorld, vec2 Pos, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_ROTATING_BALL, Pos)
{
	m_Owner = Owner;
	m_Pos = Pos;

	m_IsRotating = true;
	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_TeamMask = pOwnerChar ? pOwnerChar->TeamMask() : CClientMask();

	m_RotateDelay = Server()->TickSpeed() + 10;
	m_LaserDirAngle = 0;
	m_LaserInputDir = 0;
    
	m_TableDirV[0][0] = 5;
	m_TableDirV[0][1] = 12;
	m_TableDirV[1][0] = -12;
	m_TableDirV[1][1] = -5;

	m_ID2 = Server()->SnapNewId();
	GameWorld()->InsertEntity(this);
}

void CRotatingBall::Reset()
{
	Server()->SnapFreeId(m_ID2);
	GameWorld()->RemoveEntity(this);
}

void CRotatingBall::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if(!pOwner || !pOwner->GetPlayer()->m_RotatingBall)
	{
		Reset();
		return;
	}

	m_Pos = pOwner->GetPos();
	m_TeamMask = pOwner->TeamMask();

	m_RotateDelay--;
	if(m_RotateDelay <= 0)
	{
		m_IsRotating ^= true;

		int DirSelect = rand() % 2;
		m_LaserInputDir = rand() % (m_TableDirV[DirSelect][1] - m_TableDirV[DirSelect][0] + 1) + m_TableDirV[DirSelect][0];
		m_RotateDelay   = m_IsRotating ? Server()->TickSpeed() + (rand() % (7 - 3 + 1) + 3) : Server()->TickSpeed() + (rand() % (20 - 5 + 1) + 5);
	}   

	
	if(m_IsRotating)
		m_LaserDirAngle += m_LaserInputDir;
          
	m_LaserPos.x = pOwner->GetPos().x + 65 * sin(m_LaserDirAngle * pi/180.0f);
	m_LaserPos.y = pOwner->GetPos().y + 65 * cos(m_LaserDirAngle * pi/180.0f);

	m_ProjPos.x = m_LaserPos.x + 20 * sin(Server()->Tick()*13 * pi/180.0f);
	m_ProjPos.y = m_LaserPos.y + 20 * cos(Server()->Tick()*13 * pi/180.0f);
}

void CRotatingBall::Snap(int SnappingClient)
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

	CNetObj_DDNetLaser *pLaser = static_cast<CNetObj_DDNetLaser *>(Server()->SnapNewItem(NETOBJTYPE_DDNETLASER, GetId(), sizeof(CNetObj_DDNetLaser)));
	if(!pLaser)
		return;

	pLaser->m_ToX = round_to_int(m_LaserPos.x);
	pLaser->m_ToY = round_to_int(m_LaserPos.y);
	pLaser->m_FromX = round_to_int(m_LaserPos.x);
	pLaser->m_FromY = round_to_int(m_LaserPos.y);
	pLaser->m_StartTick = Server()->Tick();

	CNetObj_Projectile *pProj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_ID2, sizeof(CNetObj_Projectile)));
	if(!pProj)
		return;

	pProj->m_X = round_to_int(m_ProjPos.x);
	pProj->m_Y = round_to_int(m_ProjPos.y);
	pProj->m_VelX = 0;
	pProj->m_VelY = 0;
	pProj->m_StartTick = 0;
	pProj->m_Type = WEAPON_HAMMER;
}
