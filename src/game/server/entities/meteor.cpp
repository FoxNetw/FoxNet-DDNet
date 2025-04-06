#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include "character.h"
#include "meteor.h"

CMeteor::CMeteor(CGameWorld *pGameWorld, vec2 Pos, int Owner)
: CStableProjectile(pGameWorld, WEAPON_SHOTGUN, Owner, Pos, true)
{
	m_Vel = vec2(0.1f, 0.1f);
	m_Owner = Owner;
}

void CMeteor::Reset()
{
	GameWorld()->RemoveEntity(this);
}

void CMeteor::Tick()
{
	CCharacter* pChr = GameServer()->GetPlayerChar(m_Owner);
	if(!pChr || !pChr->GetPlayer()->m_Meteors)
	{
		Reset();
	}
	float Friction = GameServer()->Tuning()->m_MeteorFriction / 1000000.f;
	float MaxAccel = GameServer()->Tuning()->m_MeteorMaxAccel / 1000.f;
	float AccelPreserve = GameServer()->Tuning()->m_MeteorAccelPreserve / 1000.f;

	if(pChr)
	{
		vec2 CharPos = pChr->GetPos();
		m_Vel += normalize(CharPos - m_Pos) * (MaxAccel*AccelPreserve / (distance(CharPos, m_Pos) + AccelPreserve));
	}
	m_Pos += m_Vel;
	m_Vel *= 1.f - Friction;
}
