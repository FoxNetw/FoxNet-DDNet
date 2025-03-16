#ifndef GAME_SERVER_ENTITIES_SPECIAL_SHIELD_H
#define GAME_SERVER_ENTITIES_SPECIAL_SHIELD_H

#include <game/server/entity.h>

class CHeadItem : public CEntity
{
	vec2 m_aPos;

	CClientMask m_TeamMask;
	int m_Owner;
	float m_Dist;
	int m_aId;

	int m_Type;

public:
	CHeadItem(CGameWorld *pGameWorld, vec2 Pos, int Owner, int Type = 1);

	virtual void Reset() override;
	virtual void Tick() override;
	virtual void Snap(int SnappingClient) override;
};

#endif // GAME_SERVER_ENTITIES_SPECIAL_SHIELD_H
