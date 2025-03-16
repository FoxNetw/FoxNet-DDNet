#ifndef GAME_SERVER_ENTITIES_SPECIAL_STAFF_IND_H
#define GAME_SERVER_ENTITIES_SPECIAL_STAFF_IND_H

#include <game/server/entity.h>

class CAbilityInd : public CEntity
{
	vec2 m_aPos;

	CClientMask m_TeamMask;
	int m_Owner;
	int m_aId;

public:
	CAbilityInd(CGameWorld *pGameWorld, vec2 Pos, int Owner);

	virtual void Reset() override;
	virtual void Tick() override;
	virtual void Snap(int SnappingClient) override;
};

#endif // GAME_SERVER_ENTITIES_SPECIAL_STAFF_IND_H
