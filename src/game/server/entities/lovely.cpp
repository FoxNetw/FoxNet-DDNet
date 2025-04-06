#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include "character.h"
#include "lovely.h"

CLovely::CLovely(CGameWorld *pGameWorld, vec2 Pos, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LOVELY, Pos)
{
	m_Owner = Owner;
	m_SpawnDelay = 0;
	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_TeamMask = pOwnerChar ? pOwnerChar->TeamMask() : CClientMask();
	for (int i = 0; i < MAX_HEARTS; i++)
		m_aLovelyData[i].m_ID = Server()->SnapNewId();
	GameWorld()->InsertEntity(this);
}

void CLovely::Reset()
{
	for (int i = 0; i < MAX_HEARTS; i++)
		Server()->SnapFreeId(m_aLovelyData[i].m_ID);
	GameWorld()->RemoveEntity(this);
}

void CLovely::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if (!pOwner || !pOwner->GetPlayer()->m_Lovely)
	{
		Reset();
		return;
	}

	m_Pos = pOwner->GetPos();
	m_TeamMask = pOwner->TeamMask();

	m_SpawnDelay--;
	if (m_SpawnDelay <= 0)
	{
		SpawnNewHeart();
		int SpawnTime = 45;
		m_SpawnDelay = Server()->TickSpeed() - (rand() % (SpawnTime - (SpawnTime - 10) + 1) + (SpawnTime - 10));
	}

	for (int i = 0; i < MAX_HEARTS; i++)
	{
		if (m_aLovelyData[i].m_Lifespan == -1)
			continue;

		m_aLovelyData[i].m_Lifespan--;
		m_aLovelyData[i].m_Pos.y -= 5.f;

		if (m_aLovelyData[i].m_Lifespan == 0 || GameServer()->Collision()->TestBox(m_aLovelyData[i].m_Pos, vec2(14.f, 14.f)))
			m_aLovelyData[i].m_Lifespan = -1;
	}
}

void CLovely::SpawnNewHeart()
{
	for (int i = 0; i < MAX_HEARTS; i++)
	{
		if (m_aLovelyData[i].m_Lifespan > 0)
			continue;

		CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
		m_aLovelyData[i].m_Lifespan = Server()->TickSpeed() / 2;
		m_aLovelyData[i].m_Pos = vec2(pOwner->GetPos().x + (rand() % 50 - 25), pOwner->GetPos().y - 30);
		pOwner->SetEmote(EMOTE_HAPPY, Server()->Tick() + Server()->TickSpeed());
		break;
	}
}

void CLovely::Snap(int SnappingClient)
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if(pOwner && pOwner->IsPaused())
		return;

	if(GameServer()->GetPlayerChar(SnappingClient) && pOwner)
		if(!GameServer()->GetPlayerChar(m_Owner)->CanSnapCharacter(SnappingClient))
			return;

	if(GameServer()->GetPlayerChar(m_Owner)->GetPlayer()->m_Vanish && SnappingClient != GameServer()->GetPlayerChar(m_Owner)->GetPlayer()->GetCid() && SnappingClient != -1)
		if(!GameServer()->m_apPlayers[SnappingClient]->m_Vanish && Server()->GetAuthedState(SnappingClient) < AUTHED_OWNER)
			return;

	for (int i = 0; i < MAX_HEARTS; i++)
	{
		if (m_aLovelyData[i].m_Lifespan == -1)
			continue;

		int Size = Server()->IsSixup(SnappingClient) ? sizeof(CNetObj_DDNetPickup) : 4 * 4;
		CNetObj_DDNetPickup *pP = static_cast<CNetObj_DDNetPickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_aLovelyData[i].m_ID, Size));
		if (!pP)
			return;

		pP->m_X = round_to_int(m_aLovelyData[i].m_Pos.x);
		pP->m_Y = round_to_int(m_aLovelyData[i].m_Pos.y);
		if(Server()->IsSixup(SnappingClient))
			pP->m_Type = POWERUP_HEALTH;
		else
		{
			pP->m_Type = POWERUP_HEALTH;
			((int *)pP)[3] = 0;
		}
	}
}
