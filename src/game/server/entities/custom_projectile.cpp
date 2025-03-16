// made by fokkonaut


#include "custom_projectile.h"
#include "character.h"
#include <game/server/player.h>

#include <game/server/gamecontext.h>
#include <game/server/teams.h>
#include <game/server/gamemodes/DDRace.h>
#include <game/generated/protocol.h>
#include <game/generated/server_data.h>

#include <engine/shared/config.h>
#include <game/server/foxnet_types.h>

CCustomProjectile::CCustomProjectile(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir,
	bool Explosive, bool Freeze, bool Unfreeze, int Type, float Lifetime, float Accel, float Speed)
		: CEntity(pGameWorld, CGameWorld::ENTTYPE_CUSTOM_PROJECTILE, Pos)
{
	m_Owner = Owner;
	m_Pos = Pos;
	m_Core = normalize(Dir) * Speed;
	m_Freeze = Freeze;
	m_Explosive = Explosive;
	m_Unfreeze = Unfreeze;
	m_Direction = Dir;
	m_EvalTick = Server()->Tick();
	m_LifeTime = Server()->TickSpeed() * Lifetime;
	m_Type = Type;
	m_Accel = Accel;

	m_PrevPos = m_Pos;

	GameWorld()->InsertEntity(this);
}

void CCustomProjectile::Reset()
{
	GameWorld()->RemoveEntity(this);
}

void CCustomProjectile::Tick()
{
	m_pOwner = 0;
	if (GameServer()->GetPlayerChar(m_Owner))
		m_pOwner = GameServer()->GetPlayerChar(m_Owner);

	if (m_Owner >= 0 && !m_pOwner && Config()->m_SvDestroyBulletsOnDeath)
	{
		Reset();
		return;
	}

	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_TeamMask = pOwnerChar ? pOwnerChar->TeamMask() : CClientMask();

	m_LifeTime--;
	if (m_LifeTime <= 0)
	{
		Reset();
		return;
	}

	Move();
	HitCharacter();

	if (GameServer()->Collision()->IsSolid(m_Pos.x, m_Pos.y))
	{
		if (m_Explosive)
		{
			GameServer()->CreateExplosion(m_Pos, m_Owner, m_Type, m_Owner == -1, m_pOwner ? m_pOwner->Team() : -1, m_TeamMask);
			GameServer()->CreateSound(m_Pos, SOUND_GRENADE_EXPLODE, m_TeamMask);
		}

		if (m_CollisionState == NOT_COLLIDED)
			m_CollisionState = COLLIDED_ONCE;
	}
	else if(m_CollisionState == COLLIDED_ONCE)
		m_CollisionState = COLLIDED_TWICE;

	// weapon teleport
	int x = GameServer()->Collision()->GetIndex(m_PrevPos, m_Pos);
	int z;
	if (Config()->m_SvOldTeleportWeapons)
		z = GameServer()->Collision()->IsTeleport(x);
	else
		z = GameServer()->Collision()->IsTeleportWeapon(x);
	if (z && ((CGameControllerDDRace*)GameServer()->m_pController)->m_TeleOuts[z - 1].size())
	{
		int Num = ((CGameControllerDDRace*)GameServer()->m_pController)->m_TeleOuts[z - 1].size();
		m_Pos = ((CGameControllerDDRace*)GameServer()->m_pController)->m_TeleOuts[z - 1][(!Num) ? Num : rand() % Num];
		m_EvalTick = Server()->Tick();
	}

	m_PrevPos = m_Pos;
}

void CCustomProjectile::Move()
{
	m_Pos += m_Core;
	m_Core *= m_Accel;
}

void CCustomProjectile::HitCharacter()
{
	vec2 NewPos = m_Pos + m_Core;
	CCharacter* pHit = GameWorld()->IntersectCharacter(m_PrevPos, NewPos, 6.0f, NewPos, m_pOwner, m_Owner);
	if(!pHit)
		return;

	if(m_Freeze)
		pHit->Freeze();
	else if(m_Unfreeze)
		pHit->UnFreeze();

	if(m_Explosive)
	{
		GameServer()->CreateExplosion(m_Pos, m_Owner, m_Type, m_Owner == -1, pHit->Team(), m_TeamMask);
		GameServer()->CreateSound(m_Pos, SOUND_GRENADE_EXPLODE, m_TeamMask);
	}
	//else
		//pHit->TakeDamage(vec2(0, 0), g_pData->m_Weapons.m_aId[GameServer()->GetWeaponType(m_Type)].m_Damage, m_Owner, m_Type);

	if(GameServer()->GetPlayerChar(m_Owner)->GetActiveWeapon() == WEAPON_HEART_GUN || GameServer()->GetPlayerChar(m_Owner)->GetPlayer()->m_Ability == TYPE_HEART)
	{
		pHit->SetEmote(EMOTE_HAPPY, Server()->Tick() + 2 * Server()->TickSpeed());
		GameServer()->SendEmoticon(pHit->GetPlayer()->GetCid(), EMOTICON_HEARTS, -1);
	}

	Reset();
}

void CCustomProjectile::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(GameServer()->GetPlayerChar(SnappingClient) && pOwner)
		if(!GameServer()->GetPlayerChar(m_Owner)->CanSnapCharacter(SnappingClient))
			return;

	int Size = !Server()->IsSixup(SnappingClient) ? 4 * 4 : sizeof(CNetObj_Pickup);
	CNetObj_Pickup *pPickup = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, GetId(), Size));
	if(!pPickup)
		return;

	pPickup->m_X = (int)m_Pos.x;
	pPickup->m_Y = (int)m_Pos.y;
	pPickup->m_Type = m_Type;
	if(!Server()->IsSixup(SnappingClient))
		((int *)pPickup)[3] = 0;
}
