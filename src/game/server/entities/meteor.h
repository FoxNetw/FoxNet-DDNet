#ifndef GAME_SERVER_ENTITIES_METEOR_H
#define GAME_SERVER_ENTITIES_METEOR_H

#include <game/server/entities/stable_projectile.h>

class CMeteor : public CStableProjectile
{
	vec2 m_Vel;
	int m_Owner;

public:
	CMeteor(CGameWorld *pGameWorld, vec2 Pos, int Owner);
	
	virtual void Reset();
	virtual void Tick();
};

#endif
