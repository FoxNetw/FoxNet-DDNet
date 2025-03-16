#ifndef GAME_SERVER_ENTITIES_ABILITY_INDF4_H
#define GAME_SERVER_ENTITIES_ABILITY_INDF4_H

#include <game/server/entity.h>

class CAbilityIndF4 : public CEntity
{
	vec2 m_aPos;

	CClientMask m_TeamMask;
	vec2 m_Offset;
	int m_Owner;
	int m_aId;
	int m_Type;


public:
	CAbilityIndF4(CGameWorld *pGameWorld, vec2 Pos, int Owner, int Type);

	virtual void Reset() override;
	virtual void Tick() override;
	virtual void Snap(int SnappingClient) override;
};

#endif // GAME_SERVER_ENTITIES_ABILITY_INDF4_H