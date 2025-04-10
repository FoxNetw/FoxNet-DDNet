#ifndef GAME_SERVER_ENTITIES_ROTATING_BALL_H
#define GAME_SERVER_ENTITIES_ROTATING_BALL_H

#include <game/server/entity.h>

class CRotatingBall : public CEntity
{
	CClientMask m_TeamMask;
	int m_Owner;
	int m_ID2;

	int m_RotateDelay;
	int m_LaserDirAngle;
	int m_LaserInputDir;
	bool m_IsRotating;

	vec2 m_LaserPos;
	vec2 m_ProjPos;

	int m_TableDirV[2][2];

public:
	CRotatingBall(CGameWorld *pGameWorld, vec2 Pos, int Owner);

	virtual void Reset() override;
	virtual void Tick() override;
	virtual void Snap(int SnappingClient) override;
};

#endif // GAME_SERVER_ENTITIES_ROTATING_BALL_H
