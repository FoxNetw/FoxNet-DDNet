#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include "character.h"
#include "epic_circle.h"

CEpicCircle::CEpicCircle(CGameWorld *pGameWorld, vec2 Pos, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE, Pos)
{
	m_Owner = Owner;
	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_TeamMask = pOwnerChar ? pOwnerChar->TeamMask() : CClientMask();

	for(int i = 0; i < MAX_PARTICLES; i ++)
		m_aIds[i] = Server()->SnapNewId();
	GameWorld()->InsertEntity(this);
}

void CEpicCircle::Reset()
{
	for(int i = 0; i < MAX_PARTICLES; i++)
		Server()->SnapFreeId(m_aIds[i]);
	GameWorld()->RemoveEntity(this);
}

void CEpicCircle::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if(!pOwner || !pOwner->GetPlayer()->m_EpicCircle)
	{
		Reset();
		return;
	}

	m_Pos = pOwner->GetPos();
	m_TeamMask = pOwner->TeamMask();

	for(int i = 0; i < MAX_PARTICLES; i++)
	{
		float rad = 16.0f * powf(sinf(Server()->Tick() / 30.0f), 3) * 1 + 50;
		float TurnFac = 0.025f;
		m_RotatePos[i].x = cosf(2 * pi * (i / (float)MAX_PARTICLES) + Server()->Tick()*TurnFac) * rad;
		m_RotatePos[i].y = sinf(2 * pi * (i / (float)MAX_PARTICLES) + Server()->Tick()*TurnFac) * rad;
	}
}

void CEpicCircle::Snap(int SnappingClient)
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

	CNetObj_Projectile *pParticle[MAX_PARTICLES];
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		pParticle[i] = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_aIds[i], sizeof(CNetObj_Projectile)));
		if (pParticle[i])
		{
			pParticle[i]->m_X = m_Pos.x + m_RotatePos[i].x;
			pParticle[i]->m_Y = m_Pos.y + m_RotatePos[i].y;
			pParticle[i]->m_VelX = 0;
			pParticle[i]->m_VelY = 0;
			pParticle[i]->m_StartTick = 0;
			pParticle[i]->m_Type = WEAPON_HAMMER;
		}
	}
}
