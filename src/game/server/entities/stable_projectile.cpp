#include <game/server/gamecontext.h>
#include "stable_projectile.h"
#include <game/server/teams.h>
#include <game/server/player.h>
#include "character.h"

CStableProjectile::CStableProjectile(CGameWorld *pGameWorld, int Type, int Owner, vec2 Pos, bool HideOnSpec, bool OnlyShowOwner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_STABLE_PROJECTILE, Pos)
{
	m_Type = GameServer()->GetWeaponType(Type);
	m_Pos = Pos;
	m_LastResetPos = Pos;
	m_Owner = Owner;
	m_HideOnSpec = HideOnSpec;
	m_LastResetTick = Server()->Tick();
	m_CalculatedVel = false;
	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_TeamMask = pOwnerChar ? pOwnerChar->TeamMask() : CClientMask();
	m_OnlyShowOwner = OnlyShowOwner;

	GameWorld()->InsertEntity(this);
}

void CStableProjectile::Reset()
{
	GameWorld()->RemoveEntity(this);
}

void CStableProjectile::TickDeferred()
{
	if(Server()->Tick()%4 == 1)
	{
		m_LastResetPos = m_Pos;
		m_LastResetTick = Server()->Tick();
	}
	m_CalculatedVel = false;
}

void CStableProjectile::CalculateVel()
{
	float Time = (Server()->Tick()-m_LastResetTick)/(float)Server()->TickSpeed();
	float Curvature = 0;
	float Speed = 0;

	switch(m_Type)
	{
		case WEAPON_GRENADE:
			Curvature = GameServer()->Tuning()->m_GrenadeCurvature;
			Speed = GameServer()->Tuning()->m_GrenadeSpeed;
			break;

		case WEAPON_SHOTGUN:
			Curvature = GameServer()->Tuning()->m_ShotgunCurvature;
			Speed = GameServer()->Tuning()->m_ShotgunSpeed;
			break;

		case WEAPON_GUN:
			Curvature = GameServer()->Tuning()->m_GunCurvature;
			Speed = GameServer()->Tuning()->m_GunSpeed;
			break;
	}

	m_VelX = ((m_Pos.x - m_LastResetPos.x)/Time/Speed) * 100;
	m_VelY = ((m_Pos.y - m_LastResetPos.y)/Time/Speed - Time*Speed*Curvature/10000) * 100;

	m_CalculatedVel = true;
}

void CStableProjectile::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if(m_HideOnSpec && pOwner && pOwner->IsPaused())
		return;

	if(GameServer()->GetPlayerChar(SnappingClient) && pOwner)
		if(!GameServer()->GetPlayerChar(m_Owner)->CanSnapCharacter(SnappingClient))
			return;

	if(GameServer()->GetPlayerChar(m_Owner)->GetPlayer()->m_Vanish && SnappingClient != GameServer()->GetPlayerChar(m_Owner)->GetPlayer()->GetCid() && SnappingClient != -1)
		if(!GameServer()->m_apPlayers[SnappingClient]->m_Vanish && Server()->GetAuthedState(SnappingClient) < AUTHED_OWNER)
			return;

	if (m_Owner != -1 && !GameServer()->m_apPlayers[m_Owner])
	{
		Reset();
		return;
	}

	if (m_OnlyShowOwner && SnappingClient != m_Owner)
		return;

	CNetObj_Projectile *pProj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, GetId(), sizeof(CNetObj_Projectile)));
	if(!pProj)
		return;

	if(!m_CalculatedVel)
		CalculateVel();

	pProj->m_X = round_to_int(m_LastResetPos.x);
	pProj->m_Y = round_to_int(m_LastResetPos.y);
	pProj->m_VelX = m_VelX;
	pProj->m_VelY = m_VelY;
	pProj->m_StartTick = m_LastResetTick;
	pProj->m_Type = m_Type;
}
