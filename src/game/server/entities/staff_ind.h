#ifndef GAME_SERVER_ENTITIES_SPECIAL_STAFF_IND_H
#define GAME_SERVER_ENTITIES_SPECIAL_STAFF_IND_H

#include <game/server/entity.h>

class CStaffInd : public CEntity
{
	enum
	{
		BALL,
		ARMOR,
		BALL_FRONT,
		NUM_IDS
	};

	int m_aIds[NUM_IDS];
	vec2 m_aPos[2];

	CClientMask m_TeamMask;
	int m_Owner;
	float m_Dist;
	bool m_BallFirst;

public:
	CStaffInd(CGameWorld *pGameWorld, vec2 Pos, int Owner);

	virtual void Reset() override;
	virtual void Tick() override;
	virtual void Snap(int SnappingClient) override;
};

#endif // GAME_SERVER_ENTITIES_SPECIAL_STAFF_IND_H
